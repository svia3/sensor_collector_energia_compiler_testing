#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "ti_drivers_config.h"
#include "macTask.h"
//#include <application/FifteenDotFour/src/FifteenDotFourCollector.h>
#include <FifteenDotFourDevice.h>
#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

//#include "mac_user_config.h"
#include "macconfig.h"
#include "nvocmp.h"
#include <Energia.h>
#include <ti/runtime/wiring/cc13x2/variants/CC1312R1_LAUNCHXL/pins_energia.h>
#include <WiFi.h>
#include <stdio.h>

extern ApiMac_sAddrExt_t ApiMac_extAddr;

/* -------------------------- */
/*    function prototypes     */
/* -------------------------- */
uint32_t sensorCurrentTime();
void setup_sensor();
void loop_sensor();

/* -------------------------- */
/* Create global 15.4 instance*/
/* -------------------------- */
FifteenDotFourDevice Sensor;

uint32_t sensorCurrentTime()
{
    return Clock_getTicks() / (1000 / Clock_tickPeriod);
}

void setup_sensor()
{

    Serial.begin(115200);
    Serial.println("Starting . . .");

    /* Create macTask and semaphore */
    Sensor.begin();

    /* Default Invalid panID */
    uint16_t SPANID = 0xFFFF;
    Sensor.setPanID(SPANID);

    /* Switch to joining state */
    Sensor.connect();

    /* Loop until connected */
    while(!Sensor.connected()) {
        Sensor.process();
        Task_sleep(100);
    }
}

void loop_sensor()
{

    /* Kick off application - Forever loop */
    static uint32_t now = 0, then = 0;
    now = sensorCurrentTime();

    /* Process data request every 5 seconds */
    if((now - then) > 5000)
    {

       /* Flush our tx_buffer; geting ready to send */
       Sensor.beginTransmission(1);

       /* Create some data */
    //           char msg[] = "Hello World from Collector Node!";
    //           char addr[] = "Short Address: 0xABAB";
       char msg[] = "Hello World from Sensor Node!\n";
       char addr[] = "Short Address: 0x0001\0";

       /* Pass to tx_buffer to MAC Transmission */
       Sensor.write((const uint8_t *)msg, sizeof(msg));
       Sensor.write((const uint8_t *)addr, sizeof(addr));


       /* Signal to 15.4 that we are ready to send -> to collector */
       uint16_t destShortAddr = 0xABAB;
       bool error = Sensor.endTransmission(destShortAddr);

       if(!error)
       {
           Serial.println("Failed to send msg");
       }

       then = sensorCurrentTime();
    }

    uint8_t available = Sensor.available();
    if(available)
    {
       /* User creates a local buffer */
       char buf[100];

       /* Take data from our rx_buffer, user now has access to it */
       Sensor.read((uint8_t *)buf, available);

       /* This prints "Hello, World!" to Serial console, until EOS terminated */
       Serial.println(buf);
    }

    /* Continue with MAC processing */
    Sensor.process();

}
