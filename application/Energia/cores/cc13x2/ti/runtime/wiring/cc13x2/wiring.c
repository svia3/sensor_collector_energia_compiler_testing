/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define DEVICE_FAMILY cc13x0

#include <ti/runtime/wiring/Energia.h>
#include <ti/runtime/wiring/wiring_private.h>

#include <xdc/runtime/Types.h>

#include <ti/sysbios/family/arm/m3/TimestampProvider.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

static uint32_t timestampFreq = 48000000;
static Types_Timestamp64 clockTickTimestamp;
static uint32_t clockTickCount;

/*
 *  ======== energiaTimeSync ========
 *  Synchronize Clock ticks with Timestamp
 *
 *  Done on every Clock tick interrupt and on
 *  wake up from DEEP SLEEP.
 *
 *  To provide adequate micros() resolution, the
 *  SysTick timer is used. But, while in DEEP SLEEP,
 *  the clock to the SysTick is suppressed. 
 *  The Clock tick is maintained during
 *  deep sleep but doesn't provide adequate micros()
 *  resolution.
 *
 *  The time base for Clock (the RTC) is async
 *  with the time base for the Timestamp (the SysTick).
 *  After a long period of time without DEEP SLEEP.
 *  the Timestamp and Clock tick will diverge unless
 *  corrected each Clock tick interrupt.
 */
void energiaTimeSync(void)
{
    uint32_t key;

    key = Hwi_disable();

    TimestampProvider_get64(&clockTickTimestamp);
    clockTickCount = Clock_getTicks();

    Hwi_restore(key);
}

/*
 *  ======== micros64 ========
 *  compute delta T since timestamp snapshot.
 *  add to that the clock tick snapshot in microseconds
 */
static uint64_t micros64(void)
{
    uint64_t t0, tnow, micros;
    Types_Timestamp64 tsn;
    uint32_t key;

    key = Hwi_disable();

    TimestampProvider_get64(&tsn);

    t0 = (uint64_t)clockTickTimestamp.hi << 32 | clockTickTimestamp.lo;

    Hwi_restore(key);

    tnow = (uint64_t)tsn.hi << 32 | tsn.lo;

    micros = (tnow - t0) * (uint64_t)1000000;
    micros /= timestampFreq;

    micros += (uint64_t)clockTickCount * (uint64_t)Clock_tickPeriod;

    return(micros); 
}

/*
 *  ======== micros ========
 */
unsigned long micros()
{
    return (micros64());
}

/*
 *  ======== millis ========
 */
unsigned long millis(void)
{
    return (micros64() / 1000);
}

/*
 *  ======== delayMicroseconds ========
 *  Delay for the given number of microseconds
 */
void delayMicroseconds(unsigned int us)
{
    if (us <7) {
        //The overhead in calling and returning from this function takes about 6us
    }
    else if (us <=20) {
        int time;
        for (time = 5*(us-6); time > 0; time--) {
            asm("   nop");
        }
    }
    else if (us < 70) {
        int time;
        for (time = 5*us; time > 0; time--) {
            asm("   nop");
        }
    }
    else {
        uint32_t t0, deltaT;
        Types_FreqHz freq;

        TimestampProvider_getFreq(&freq);
        deltaT = us * (freq.lo/1000000);

        t0 = TimestampProvider_get32();

        while ((TimestampProvider_get32()-t0) < deltaT) {
            ;
        }
    }
}

/*
 *  ======== delay ========
 */
void delay(uint32_t milliseconds)
{
    uint64_t delay;

    if (milliseconds == 0) {
        Task_yield();
        return;
    }

    /* math below assumes tickPeriod is <= 1000 */
    delay = ((uint64_t)milliseconds * (uint64_t)1000) / (uint64_t)Clock_tickPeriod;

    Task_sleep(delay);
}

/*
 * The following two functions implement a hack to
 * force the Clock functions to be executed within
 * the Clock timer interrupt thread rather than
 * within a Clock Swi so that UART_write() can be called within
 * an ISR thread.
 * Without this hack, the tx complete callback is called from within
 * a Swi context and Swis are only executed after all Hwis are finished.
 * Consequently, the UART_write() callback will never get called while
 * the current thread is an ISR.
 */
void myClock_doTick(UArg arg) {
    energiaTimeSync();
    Clock_workFuncDynamic(0,0);
}

/*
 * reroute cc26xx Timer func to "myClock_doTick"
 * Make cc26xx Timer interrupt higher priority than GPIO interrupts
 * so that they will nest user "attachInterrupt()" pin interrupts.
 */
void energiaLastFxn() {
    Hwi_setPriority(20, 0xc0);
    Clock_TimerProxy_setFunc(Clock_getTimerHandle(), myClock_doTick, 0);
}

/*
 *  ======== getSpiInfo ========
 *
 *  A hack to work around spiPolling only being supported
 *  in SPI_MODE_BLOCKING. Remove if/when this is resolved
 *  in the SPI drivers.
 */
void getSpiInfo(void *spi, SpiInfo *spiInfo)
{
    SPICC26XXDMA_Object *obj;
    SPICC26XXDMA_HWAttrsV1 const *hwAttrs;
    SPI_Handle spiHandle = (SPI_Handle)spi;

    obj = (SPICC26XXDMA_Object *)(spiHandle->object);
    hwAttrs = (SPICC26XXDMA_HWAttrsV1 *)(spiHandle->hwAttrs);

    spiInfo->transferModePtr = &obj->transferMode;
    spiInfo->minDmaTransferSize = hwAttrs->minDmaTransferSize;
}
