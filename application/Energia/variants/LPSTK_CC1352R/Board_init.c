/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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

/*
 *  ====================== Board.c =============================================
 *  This file is responsible for setting up the board specific items for the
 *  CC1352 LaunchPad.
 */

#define DEVICE_FAMILY cc13x2

/*
 *  ====================== Includes ============================================
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/devices/cc13x2_cc26x2/driverlib/ioc.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/udma.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_ints.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include "Board.h"

/*
 *  =============================== ADC ===============================
 */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCCC26XX.h>

void ADCCC26XX_close(ADC_Handle handle);
int_fast16_t ADCCC26XX_control(ADC_Handle handle, uint_fast16_t cmd, void *arg);
int_fast16_t ADCCC26XX_convert(ADC_Handle handle, uint16_t *value);
uint32_t ADCCC26XX_convertRawToMicroVolts(ADC_Handle handle,
    uint16_t rawAdcValue);
void ADCCC26XX_init(ADC_Handle handle);
ADC_Handle ADCCC26XX_open(ADC_Handle handle, ADC_Params *params);

/* ADC function table for ADCCC26XX implementation */
const ADC_FxnTable myADCCC26XX_fxnTable = {
    ADCCC26XX_close,
    NULL, /* ADCCC26XX_control, */
    ADCCC26XX_convert,
    NULL, /* ADCCC26XX_convertRawToMicroVolts, */
    ADCCC26XX_init,
    ADCCC26XX_open
};

ADCCC26XX_Object adcCC26xxObjects[LPSTK_CC1352R_ADCCOUNT];

const ADCCC26XX_HWAttrs adcCC26xxHWAttrs[LPSTK_CC1352R_ADCCOUNT] = {
    {
        .adcDIO              = LPSTK_CC1352R_DIO23_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO7,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO24_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO6,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO25_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO5,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO26_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO4,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO27_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO3,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO28_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO2,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO29_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO1,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = LPSTK_CC1352R_DIO30_ANALOG,
        .adcCompBInput       = ADC_COMPB_IN_AUXIO0,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = PIN_UNASSIGNED,
        .adcCompBInput       = ADC_COMPB_IN_DCOUPL,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = PIN_UNASSIGNED,
        .adcCompBInput       = ADC_COMPB_IN_VSS,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    },
    {
        .adcDIO              = PIN_UNASSIGNED,
        .adcCompBInput       = ADC_COMPB_IN_VDDS,
        .refSource           = ADCCC26XX_VDDS_REFERENCE,
        .samplingDuration    = ADCCC26XX_SAMPLING_DURATION_2P7_US,
        .inputScalingEnabled = true,
        .triggerSource       = ADCCC26XX_TRIGGER_MANUAL,
        .returnAdjustedVal   = true
    }
};

const ADC_Config ADC_config[LPSTK_CC1352R_ADCCOUNT] = {
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC0], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC0]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC1], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC1]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC2], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC2]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC3], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC3]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC4], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC4]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC5], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC5]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC6], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC6]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADC7], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADC7]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADCDCOUPL], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADCDCOUPL]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADCVSS], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADCVSS]},
    {&myADCCC26XX_fxnTable, &adcCC26xxObjects[LPSTK_CC1352R_ADCVDDS], &adcCC26xxHWAttrs[LPSTK_CC1352R_ADCVDDS]},
};

const uint_least8_t ADC_count = LPSTK_CC1352R_ADCCOUNT;

/*============================================================================*/

/*
 * ======== GPIO driver ========
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>

/* GPIO configuration structure definitions */

GPIO_PinConfig gpioPinConfigs[] = {
    /* port_pin */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  0  - dummy */
                          
    /* pins 1-10 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  1  - 3.3V */
    GPIOCC26XX_DIO_23 | GPIO_DO_NOT_CONFIG,     /*  2  - DIO_23 */
    GPIOCC26XX_DIO_12 | GPIO_DO_NOT_CONFIG,     /*  3  - DIO_12 */
    GPIOCC26XX_DIO_13 | GPIO_DO_NOT_CONFIG,     /*  4  - DIO_13 */
    GPIOCC26XX_DIO_22 | GPIO_DO_NOT_CONFIG,     /*  5  - DIO_22 */
    GPIOCC26XX_DIO_24 | GPIO_DO_NOT_CONFIG,     /*  6  - DIO_24 */
    GPIOCC26XX_DIO_10 | GPIO_DO_NOT_CONFIG,     /*  7  - DIO_10 */
    GPIOCC26XX_DIO_21 | GPIO_DO_NOT_CONFIG,     /*  8  - DIO_21 */
    GPIOCC26XX_DIO_04 | GPIO_DO_NOT_CONFIG,     /*  9  - DIO_04 */
    GPIOCC26XX_DIO_05 | GPIO_DO_NOT_CONFIG,     /*  10 - DIO_05 */
                          
    /* pins 11-20 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  11 - NC */
    GPIOCC26XX_DIO_14 | GPIO_DO_NOT_CONFIG,     /*  12 - DIO_14 */
    GPIOCC26XX_DIO_15 | GPIO_DO_NOT_CONFIG,     /*  13 - DIO_15 */
    GPIOCC26XX_DIO_08 | GPIO_DO_NOT_CONFIG,     /*  14 - DIO_08 */
    GPIOCC26XX_DIO_09 | GPIO_DO_NOT_CONFIG,     /*  15 - DIO_09 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  16 - LPRST */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  17 - NC */
    GPIOCC26XX_DIO_11 | GPIO_DO_NOT_CONFIG,     /*  18 - DIO_11 */
    GPIOCC26XX_DIO_03 | GPIO_DO_NOT_CONFIG,     /*  19 - DIO_03 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  20 - GND */
                          
    /* pins 21-30 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  21 - 5V */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  22 - GND */
    GPIOCC26XX_DIO_25 | GPIO_DO_NOT_CONFIG,     /*  23 - DIO_25 */
    GPIOCC26XX_DIO_26 | GPIO_DO_NOT_CONFIG,     /*  24 - DIO_26 */
    GPIOCC26XX_DIO_27 | GPIO_DO_NOT_CONFIG,     /*  25 - DIO_27 */
    GPIOCC26XX_DIO_28 | GPIO_DO_NOT_CONFIG,     /*  26 - DIO_28 */
    GPIOCC26XX_DIO_29 | GPIO_DO_NOT_CONFIG,     /*  27 - DIO_29 */
    GPIOCC26XX_DIO_30 | GPIO_DO_NOT_CONFIG,     /*  28 - DIO_30 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  29 - NC */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  30 - NC */
                          
    /* pins 31-40 */
    GPIOCC26XX_DIO_17 | GPIO_DO_NOT_CONFIG,     /*  31 - DIO_17 */
    GPIOCC26XX_DIO_16 | GPIO_DO_NOT_CONFIG,     /*  32 - DIO_16 */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  33 - TCK */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  34 - TMS */
    GPIOCC26XX_EMPTY_PIN | GPIO_DO_NOT_CONFIG,  /*  35 - BPRST */
    GPIOCC26XX_DIO_18 | GPIO_DO_NOT_CONFIG,     /*  36 - DIO_18 */
    GPIOCC26XX_DIO_19 | GPIO_DO_NOT_CONFIG,     /*  37 - DIO_19 */
    GPIOCC26XX_DIO_20 | GPIO_DO_NOT_CONFIG,     /*  38 - DIO_20 */
    GPIOCC26XX_DIO_06 | GPIO_DO_NOT_CONFIG,     /*  39 - DIO_06 */
    GPIOCC26XX_DIO_07 | GPIO_DO_NOT_CONFIG,     /*  40 - DIO_07 */
};

GPIO_CallbackFxn gpioCallbackFunctions[] = {
    /* port_pin */
    NULL,  /*  0  - dummy */

    /* pins 1-10 */
    NULL,  /*  1  - 3.3V */
    NULL,  /*  2  - DIO_23 */
    NULL,  /*  3  - DIO_03 */
    NULL,  /*  4  - DIO_02 */
    NULL,  /*  5  - DIO_22 */
    NULL,  /*  6  - DIO_24 */
    NULL,  /*  7  - DIO_10 */
    NULL,  /*  8  - DIO_21 */
    NULL,  /*  9  - DIO_04 */
    NULL,  /*  10 - DIO_05 */
                    
    /* pins 11-20 */
    NULL,  /*  11 - DIO_15 */
    NULL,  /*  12 - DIO_14 */
    NULL,  /*  13 - DIO_13 */
    NULL,  /*  14 - DIO_08 */
    NULL,  /*  15 - DIO_09 */
    NULL,  /*  16 - LPRST */
    NULL,  /*  17 - NC */
    NULL,  /*  18 - DIO_11 */
    NULL,  /*  19 - DIO_12 */
    NULL,  /*  20 - GND */
                    
    /* pins 21-30 */
    NULL,  /*  21 - 5V */
    NULL,  /*  22 - GND */
    NULL,  /*  23 - DIO_25 */
    NULL,  /*  24 - DIO_26 */
    NULL,  /*  25 - DIO_27 */
    NULL,  /*  26 - DIO_28 */
    NULL,  /*  27 - DIO_29 */
    NULL,  /*  28 - DIO_30 */
    NULL,  /*  29 - DIO_00 */
    NULL,  /*  30 - DIO_01 */
                    
    /* pins 31-40 */
    NULL,  /*  31 - DIO_17 */
    NULL,  /*  32 - DIO_16 */
    NULL,  /*  33 - TCK */
    NULL,  /*  34 - TMS */
    NULL,  /*  35 - BPRST */
    NULL,  /*  36 - DIO_18 */
    NULL,  /*  37 - DIO_19 */
    NULL,  /*  38 - DIO_20 */
    NULL,  /*  39 - DIO_06 */
    NULL,  /*  40 - DIO_07 */
};

/* User requested callback functions for the GPIO input signals */

/* The device-specific GPIO_config structure */
const GPIOCC26XX_Config GPIOCC26XX_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs)/sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = ~0
};

/*
 *  ============================ GPTimer =================================
 *  Remove unused entries to reduce flash usage both in Board.c and Board.h
 */

#include <ti/drivers/timer/GPTimerCC26XX.h>

/*  GPTimer objects, one per full-width timer (A+B) (Timer 0, Timer 1..) */
GPTimerCC26XX_Object gptimerCC26XXObjects[LPSTK_CC1352R_GPTIMERCOUNT];

/* GPTimer hardware attributes, one per timer part (Timer 0A, 0B, 1A, 1B..) */
const GPTimerCC26XX_HWAttrs gptimerCC26xxHWAttrs[LPSTK_CC1352R_GPTIMERPARTSCOUNT] = {
    { .baseAddr = GPT0_BASE, .intNum = INT_GPT0A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0A, },
    { .baseAddr = GPT0_BASE, .intNum = INT_GPT0B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT0, .pinMux = GPT_PIN_0B, },
    { .baseAddr = GPT1_BASE, .intNum = INT_GPT1A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT1, .pinMux = GPT_PIN_1A, },
    { .baseAddr = GPT1_BASE, .intNum = INT_GPT1B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT1, .pinMux = GPT_PIN_1B, },
    { .baseAddr = GPT2_BASE, .intNum = INT_GPT2A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT2, .pinMux = GPT_PIN_2A, },
    { .baseAddr = GPT2_BASE, .intNum = INT_GPT2B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT2, .pinMux = GPT_PIN_2B, },
    { .baseAddr = GPT3_BASE, .intNum = INT_GPT3A, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT3, .pinMux = GPT_PIN_3A, },
    { .baseAddr = GPT3_BASE, .intNum = INT_GPT3B, .intPriority = (~0), .powerMngrId = PowerCC26XX_PERIPH_GPT3, .pinMux = GPT_PIN_3B, },
};

/* GPTimer configuration (used as GPTimer_Handle by driver and application) */
const GPTimerCC26XX_Config GPTimerCC26XX_config[LPSTK_CC1352R_GPTIMERPARTSCOUNT] = {
    { &gptimerCC26XXObjects[0], &gptimerCC26xxHWAttrs[0], GPT_A },
    { &gptimerCC26XXObjects[0], &gptimerCC26xxHWAttrs[1], GPT_B },
    { &gptimerCC26XXObjects[1], &gptimerCC26xxHWAttrs[2], GPT_A },
    { &gptimerCC26XXObjects[1], &gptimerCC26xxHWAttrs[3], GPT_B },
    { &gptimerCC26XXObjects[2], &gptimerCC26xxHWAttrs[4], GPT_A },
    { &gptimerCC26XXObjects[2], &gptimerCC26xxHWAttrs[5], GPT_B },
    { &gptimerCC26XXObjects[3], &gptimerCC26xxHWAttrs[6], GPT_A },
    { &gptimerCC26XXObjects[3], &gptimerCC26xxHWAttrs[7], GPT_B },
};

/*
 *  ============================= I2C =====================================
 */

/* Include drivers */
#include <ti/drivers/i2c/I2CCC26XX.h>

extern void I2CCC26XX_cancel(I2C_Handle handle);
extern void I2CCC26XX_close(I2C_Handle handle);
extern int_fast16_t I2CCC26XX_control(I2C_Handle handle, uint_fast16_t cmd, void *arg);
extern void I2CCC26XX_init(I2C_Handle handle);
extern I2C_Handle I2CCC26XX_open(I2C_Handle handle, I2C_Params *params);
extern int_fast16_t I2CCC26XX_transfer(I2C_Handle handle, I2C_Transaction *transaction, uint32_t timeout);

const I2C_FxnTable myI2CCC26XX_fxnTable = {
    NULL, /* I2CCC26XX_cancel, */
    I2CCC26XX_close,
    NULL, /* I2CCC26XX_control, */
    I2CCC26XX_init,
    I2CCC26XX_open,
    I2CCC26XX_transfer
};

/* I2C objects */
I2CCC26XX_Object i2cCC26xxObjects[LPSTK_CC1352R_I2CCOUNT];

/* I2C configuration structure, describing which pins are to be used */
const I2CCC26XX_HWAttrsV1 i2cCC26xxHWAttrs[LPSTK_CC1352R_I2CCOUNT] = {
    {
        .baseAddr = I2C0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_I2C0,
        .intNum = INT_I2C_IRQ,    /* <driverlib>/inc/hw_ints.h */
        .intPriority = (~0),
        .swiPriority = 0,
        .sdaPin = Board_I2C0_SDA0,
        .sclPin = Board_I2C0_SCL0,
    },
};

const I2C_Config I2C_config[] = {
    {
        .fxnTablePtr = &myI2CCC26XX_fxnTable,
        .object = &i2cCC26xxObjects[0],
        .hwAttrs = &i2cCC26xxHWAttrs[0]
    },
};

const uint_least8_t I2C_count = LPSTK_CC1352R_I2CCOUNT;

/*
 *  =============================== NVS ===============================
 *  Non-Volatile Storage configuration.
 */
#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSCC26XX.h>
#include <ti/drivers/nvs/NVSSPI25X.h>

NVSCC26XX_Object nvsCC26XXObjects[1];
NVSSPI25X_Object nvsSPI25XObjects[1];

static uint8_t verifyBuf[64];

extern uint8_t __NVS_BASE__;
extern uint8_t __NVS_SIZE__;

NVSCC26XX_HWAttrs nvsCC26XXHWAttrs[1] = {
    {
        .regionBase = (void *)&__NVS_BASE__,
        .regionSize = (size_t)(&__NVS_SIZE__)
    },
};

NVSSPI25X_HWAttrs nvsSPI25XHWAttrs[1] = {
    {
        .regionBaseOffset = 0,
        .regionSize = 0x100000,  /* 1 MByte */
        .sectorSize = 4096,
        .verifyBuf = verifyBuf,
        .verifyBufSize = 64,
        .spiHandle = NULL,
        .spiIndex = 0,
        .spiBitRate = 4000000,
        .spiCsnGpioIndex = 38,   /* boosterpack pin 38 = DIO20 */
    },
};

const NVS_Config NVS_config[] = {
    /* Region 0 is internal flash */
    {
        &NVSCC26XX_fxnTable,
        &nvsCC26XXObjects[0],
        &nvsCC26XXHWAttrs[0]
    },
    /* Region 1 is external spi flash */
    {
        &NVSSPI25X_fxnTable,
        &nvsSPI25XObjects[0],
        &nvsSPI25XHWAttrs[0]
    },
};

int NVS_count = 2;

/*
 *  =============================== PIN ===============================
 */
#include <ti/drivers/PIN.h>
#include  <ti/drivers/pin/PINCC26XX.h>

const PIN_Config BoardGpioInitTable[] = {
    Board_RLED   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,         /* LED initially off             */
    Board_GLED   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,         /* LED initially off             */
    Board_BTN1   | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_BOTHEDGES | PIN_HYSTERESIS,            /* Button is active low          */
    Board_BTN2   | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_BOTHEDGES | PIN_HYSTERESIS,            /* Button is active low          */
    Board_UART_RX | PIN_INPUT_EN | PIN_PULLDOWN,                                              /* UART RX via debugger back channel */
    Board_UART_TX | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,                        /* UART TX via debugger back channel */
    Board_SPI0_MOSI | PIN_INPUT_EN | PIN_PULLDOWN,                                            /* SPI master out - slave in */
    Board_SPI0_MISO | PIN_INPUT_EN | PIN_PULLDOWN,                                            /* SPI master in - slave out */
    Board_SPI0_CLK | PIN_INPUT_EN | PIN_PULLDOWN,                                             /* SPI clock */

    PIN_TERMINATE
};

PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {
    .intPriority = ~0,
    .swiPriority = 0
};

/*
 *  =============================== Power ===============================
 */

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>

/*
 *  ======== PowerCC26XX_config ========
 */
const PowerCC26X2_Config PowerCC26X2_config = {
    .policyInitFxn      = NULL,
    .policyFxn          = &PowerCC26XX_standbyPolicy,
    .calibrateFxn       = &PowerCC26XX_calibrate,
    .enablePolicy       = true,
    .calibrateRCOSC_LF  = true,
    .calibrateRCOSC_HF  = true,
};

/*
 *  ============================= PWM ====================================
 *  Remove unused entries to reduce flash usage both in Board.c and Board.h
 */

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerCC26XX.h>

void PWMTimerCC26XX_close(PWM_Handle handle);
int_fast16_t PWMTimerCC26XX_control(PWM_Handle handle, uint_fast16_t cmd, void *arg);
void PWMTimerCC26XX_init(PWM_Handle handle);
PWM_Handle PWMTimerCC26XX_open(PWM_Handle handle, PWM_Params *params);
int_fast16_t PWMTimerCC26XX_setDuty(PWM_Handle handle, uint32_t dutyValue);
int_fast16_t PWMTimerCC26XX_setPeriod(PWM_Handle handle, uint32_t periodValue);
void PWMTimerCC26XX_start(PWM_Handle handle);
void PWMTimerCC26XX_stop(PWM_Handle handle);

/* PWM function table for PWMTimerCC26XX implementation */
const PWM_FxnTable myPWMTimerCC26XX_fxnTable = {
    PWMTimerCC26XX_close,
    NULL, /* PWMTimerCC26XX_control, */
    PWMTimerCC26XX_init,
    PWMTimerCC26XX_open,
    PWMTimerCC26XX_setDuty,
    NULL, /* PWMTimerCC26XX_setPeriod, */
    NULL, /* PWMTimerCC26XX_setPeriodAndDuty, */
    PWMTimerCC26XX_start,
    NULL /* PWMTimerCC26XX_stop */
};

/* PWM object, one per PWM output */
PWMTimerCC26XX_Object pwmtimerCC26xxObjects[LPSTK_CC1352R_PWMCOUNT];

/* PWM configuration, one per PWM output.   */
PWMTimerCC26XX_HwAttrs pwmtimerCC26xxHWAttrs[LPSTK_CC1352R_PWMCOUNT] = {
    { .pwmPin = Board_PWMPIN0, .gpTimerUnit = Board_GPTIMER0A },
    { .pwmPin = Board_PWMPIN1, .gpTimerUnit = Board_GPTIMER0B },
    { .pwmPin = Board_PWMPIN2, .gpTimerUnit = Board_GPTIMER1A },
    { .pwmPin = Board_PWMPIN3, .gpTimerUnit = Board_GPTIMER1B },
    { .pwmPin = Board_PWMPIN4, .gpTimerUnit = Board_GPTIMER2A },
    { .pwmPin = Board_PWMPIN5, .gpTimerUnit = Board_GPTIMER2B },
    { .pwmPin = Board_PWMPIN6, .gpTimerUnit = Board_GPTIMER3A },
    { .pwmPin = Board_PWMPIN7, .gpTimerUnit = Board_GPTIMER3B },
};

/* PWM configuration (used as PWM_Handle by driver and application) */
const PWM_Config PWM_config[LPSTK_CC1352R_PWMCOUNT] = {
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[0], &pwmtimerCC26xxHWAttrs[0] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[1], &pwmtimerCC26xxHWAttrs[1] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[2], &pwmtimerCC26xxHWAttrs[2] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[3], &pwmtimerCC26xxHWAttrs[3] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[4], &pwmtimerCC26xxHWAttrs[4] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[5], &pwmtimerCC26xxHWAttrs[5] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[6], &pwmtimerCC26xxHWAttrs[6] },
    { &myPWMTimerCC26XX_fxnTable, &pwmtimerCC26xxObjects[7], &pwmtimerCC26xxHWAttrs[7] },
};

const uint_least8_t PWM_count = LPSTK_CC1352R_PWMCOUNT;

/*
 *  ========================== SPI DMA ===================================
*/

/* Include drivers */
#include <ti/drivers/spi/SPICC26XXDMA.h>

/* SPICC26XXDMA functions */
extern void SPICC26XXDMA_close(SPI_Handle handle);
extern int_fast16_t SPICC26XXDMA_control(SPI_Handle handle, uint_fast16_t cmd, void *arg);
extern void SPICC26XXDMA_init(SPI_Handle handle);
extern SPI_Handle SPICC26XXDMA_open(SPI_Handle handle, SPI_Params *params);
extern bool SPICC26XXDMA_transfer(SPI_Handle handle, SPI_Transaction *transaction);
extern void SPICC26XXDMA_transferCancel(SPI_Handle handle);

/* SPI function table for SPICC26XXDMA implementation */
const SPI_FxnTable mySPICC26XXDMA_fxnTable = {
    SPICC26XXDMA_close,
    NULL, /* SPICC26XXDMA_control, */
    SPICC26XXDMA_init,
    SPICC26XXDMA_open,
    SPICC26XXDMA_transfer,
    NULL /* SPICC26XXDMA_transferCancel */
};

/* SPI objects */

SPICC26XXDMA_Object spiCC26XXDMAObjects[LPSTK_CC1352R_SPICOUNT];

/* SPI configuration structure, describing which pins are to be used */
const SPICC26XXDMA_HWAttrsV1 spiCC26XXDMAHWAttrs[LPSTK_CC1352R_SPICOUNT] = {
    {   /* LPSTK_CC1352R_SPI0 */
        .baseAddr           = SSI0_BASE,
        .intNum             = INT_SSI0_COMB,   /* <driverlib>/inc/hw_ints.h */
        .intPriority        = 0xC0,       /* make SPI interrupt one priority higher than default */
        .swiPriority        = 0,
        .powerMngrId        = PowerCC26XX_PERIPH_SSI0,
        .defaultTxBufValue  = 0,
        .rxChannelBitMask   = 1<<UDMA_CHAN_SSI0_RX,
        .txChannelBitMask   = 1<<UDMA_CHAN_SSI0_TX,
        .mosiPin            = Board_SPI0_MOSI,
        .misoPin            = Board_SPI0_MISO,
        .clkPin             = Board_SPI0_CLK,
        .csnPin             = PIN_UNASSIGNED,    /* External flash / DevPk uses SPI0 */
        .minDmaTransferSize = 16
    }
};

/* SPI configuration structure */
const SPI_Config SPI_config[] = {
    /* LPSTK_CC1352R_SPI0 */
    {
        .fxnTablePtr = &mySPICC26XXDMA_fxnTable, 
        .object      = &spiCC26XXDMAObjects[0], 
        .hwAttrs     = &spiCC26XXDMAHWAttrs[0]
    },
};

const uint_least8_t SPI_count = LPSTK_CC1352R_SPICOUNT;

/*
 *  ============================= UART ===================================
*/

/* Include drivers */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26X2.h>


extern void UARTCC26X2_close(UART_Handle handle);
extern int_fast16_t UARTCC26X2_control(UART_Handle handle, uint_fast16_t cmd,
        void *arg);
extern void UARTCC26X2_init(UART_Handle handle);
extern UART_Handle UARTCC26X2_open(UART_Handle handle, UART_Params *params);
extern int_fast32_t UARTCC26X2_read(UART_Handle handle, void *buffer, size_t size);
extern void UARTCC26X2_readCancel(UART_Handle handle);
extern int_fast32_t UARTCC26X2_readPolling(UART_Handle handle, void *buffer,
        size_t size);
extern int_fast32_t UARTCC26X2_write(UART_Handle handle, const void *buffer,
        size_t size);
extern void UARTCC26X2_writeCancel(UART_Handle handle);
extern int_fast32_t UARTCC26X2_writePolling(UART_Handle handle, const void *buffer,
        size_t size);

/* UART function table for UARTMP432 implementation */
const UART_FxnTable myUARTCC26X2_fxnTable = {
    UARTCC26X2_close,
    UARTCC26X2_control,
    UARTCC26X2_init,
    UARTCC26X2_open,
    UARTCC26X2_read,
    NULL, /* UARTCC26X2_readPolling, */
    NULL, /* UARTCC26X2_readCancel, */
    UARTCC26X2_write,
    NULL, /* UARTCC26XX_writePolling, */
    NULL /* UARTCC26XX_writeCancel, */
};

/* UART objects */
UARTCC26X2_Object uartCC26X2Objects[LPSTK_CC1352R_UARTCOUNT];
unsigned char uartCC26X2RingBuffer[LPSTK_CC1352R_UARTCOUNT][32];

/* UART hardware parameter structure, also used to assign UART pins */
const UARTCC26X2_HWAttrs uartCC26X2HWAttrs[LPSTK_CC1352R_UARTCOUNT] = {
    {   /* LPSTK_CC1352R_UART0 */
        .baseAddr = UART0_BASE,
        .intNum = INT_UART0_COMB,    /* <driverlib>/inc/hw_ints.h */
        .intPriority = (0xc0),
        .swiPriority = 0,
        .txPin = Board_UART_TX,
        .rxPin = Board_UART_RX,
        .ctsPin = PIN_UNASSIGNED,
        .rtsPin = PIN_UNASSIGNED,
        .ringBufPtr     = uartCC26X2RingBuffer[0],
        .ringBufSize    = sizeof(uartCC26X2RingBuffer[0]),
        .txIntFifoThr   = UARTCC26X2_FIFO_THRESHOLD_1_8,
        .rxIntFifoThr   = UARTCC26X2_FIFO_THRESHOLD_4_8
    },
};

/* UART configuration structure */
const UART_Config UART_config[] = {
    {
        .fxnTablePtr = &myUARTCC26X2_fxnTable,
        .object      = &uartCC26X2Objects[0],
        .hwAttrs     = &uartCC26X2HWAttrs[0]
    },
};

const uint_least8_t UART_count = LPSTK_CC1352R_UARTCOUNT;

/*
 *  =============================== UDMA ===============================
 */
#include <ti/drivers/dma/UDMACC26XX.h>

UDMACC26XX_Object udmaObjects[1];

const UDMACC26XX_HWAttrs udmaHWAttrs[1] = {
    {
        .baseAddr    = UDMA0_BASE,
        .powerMngrId = PowerCC26XX_PERIPH_UDMA,
        .intNum      = INT_DMA_ERR,
        .intPriority = ~0
    }
};

const UDMACC26XX_Config UDMACC26XX_config[1] = {
    {
         .object  = &udmaObjects[0],
         .hwAttrs = &udmaHWAttrs[0]
    },
};

/*
 *  =============================== Watchdog ===============================
 */
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogCC26XX.h>

WatchdogCC26XX_Object watchdogCC26XXObjects[1];

const WatchdogCC26XX_HWAttrs watchdogCC26XXHWAttrs[1] = {
    {
        .baseAddr    = WDT_BASE,
        .reloadValue = 1000 /* Reload value in milliseconds */
    },
};

const Watchdog_Config Watchdog_config[1] = {
    {
        .fxnTablePtr = &WatchdogCC26XX_fxnTable,
        .object      = &watchdogCC26XXObjects[0],
        .hwAttrs     = &watchdogCC26XXHWAttrs[0]
    },
};

const uint_least8_t Watchdog_count = 1;

/*
 *  ======== Board_initPIN ========
 */
void Board_initPIN()
{
    PIN_init(BoardGpioInitTable);
}

static Power_NotifyObj energiaNotifyObject;
extern void energiaTimeSync(void);

/*
 *  ======== Board_initPower ========
 */
void Board_initPower(void)
{
    Power_init();

    /*
     * resync micros() and millis() time bases after
     * clock to SysTick has been cut while in deep sleep
     */
    Power_registerNotify(&energiaNotifyObject, PowerCC26XX_AWAKE_STANDBY,
                         (Power_NotifyFxn)energiaTimeSync, (uintptr_t)NULL);
}

/*
 *  ======== Board_init ========
 */
void Board_init(void)
{
    /* driver-independent initialization */

    /* driver-specific initialization */
    Board_initPower();
    Board_initPIN();
    GPIO_init();
}
