#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "ti_drivers_config.h"
#include "macTask.h"
#include <application/FifteenDotFour/src/FifteenDotFourCollector.h>
#include <application/FifteenDotFour/src/FifteenDotFourDevice.h>
#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

//#include "mac_user_config.h"
#include "macconfig.h"
#include "nvocmp.h"
#include <Energia.h>
#include <ti/runtime/wiring/cc13x2/variants/CC1312R1_LAUNCHXL/pins_energia.h>
#include <WiFi.h>
#include <stdio.h>

#ifdef NV_RESTORE
mac_Config_t Main_user1Cfg =
    { 0 };
#endif

#define APP_TASK_PRIORITY   1
#define APP_TASK_STACK_SIZE 2048

extern ApiMac_sAddrExt_t ApiMac_extAddr;
Task_Struct appTask;
static uint8_t appTaskStack[APP_TASK_STACK_SIZE];

static uint8_t _macTaskId;

extern void Board_init(void);
#define SPANID 0xFFFF
#define CPANID 0xFAFA

#define COLLECTOR

#ifdef COLLECTOR
FifteenDotFourCollector myNode;
#else
FifteenDotFourDevice myNode;
#endif

uint32_t getCurrentTime()
{
    return Clock_getTicks() / (1000 / Clock_tickPeriod);
}

void printWifiData() {
  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void startWifi(const char* ssid, const char* pwd)
{
    Serial.begin(115200);
    Serial.println("Starting . . .");
//
    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);

    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to Network named: ");
    // print the network name (SSID);
    Serial.println(ssid);

    WiFi.begin((char *)ssid, (char *)pwd);

    while ( WiFi.status() != WL_CONNECTED) {
      // print dots while we wait to connect
      Serial.print(".");
      delay(300);
    }

    Serial.println("\nYou're connected to the network");
    Serial.println("Waiting for an ip address");

    while (WiFi.localIP() == INADDR_NONE) {
      // print dots while we wait for an ip addresss
      Serial.print(".");
      delay(300);
    }

    Serial.println("\nIP Address obtained");
}

void printFifteenDotFourStats()
{
    /* Printing the PAN ID */
    char msg[50];
    sprintf(msg, "Pan ID: %02X", myNode.getPanID());
    Serial.println(msg);

#ifdef COLLECTOR
    // do nothing for now
#else
    /* Connecting  */
    Serial.println("Connecting to Collector ...");
    if (myNode.connected()) Serial.println("Connected!");
        /* ----------- */
    /* Printing the Coordinator Short Address */
    sprintf(msg, "Coordinator Short Address: %02X", myNode.getCoordShortAddr());
    Serial.println(msg);
#endif
}
void appTaskFxn(UArg a0, UArg a1)
{

#ifdef COLLECTOR
    /* Connect to local Network using Energia Framework */
    const char ssid[] = "FiOS-XY4PM_EXT";
    const char pwd[] = "italy123";

    startWifi(ssid, pwd);
    printCurrentNet();
    printWifiData();
#endif

    /* Get the baked-in primary IEEE Address */
    memcpy(ApiMac_extAddr, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET),
               (APIMAC_SADDR_EXT_LEN));

    /* Create macTask and semaphore */
    myNode.begin();

#ifdef COLLECTOR
    uint16_t sendShortAddr = 0x0001;
    /* Configure to join any network by setting pan id to 0xFFFF. */
    myNode.setPanID(CPANID);
    /* Connect to any network allowing join */
    myNode.start();
#else
    uint16_t sendShortAddr = 0xABAB;

    myNode.setPanID(SPANID);

    myNode.connect();

    while(!myNode.connected()) {
        myNode.process();
        Task_sleep(100);
    }
#endif

    /* Print 15.4 network stats */
    printFifteenDotFourStats();

    /* Kick off application - Forever loop */
    uint32_t now, start;
    start = getCurrentTime();

    while(1)
    {
        now = getCurrentTime();

        /* Process data request every 5 seconds */
        if((now - start) > 5000) {

            /* Flush our tx_buffer; geting ready to send */
            myNode.beginTransmission(1);

            /* Create some data */
#ifdef COLLECTOR
            char msg[] = "Hello World from Collector Node!";
            char addr[] = "Short Address: 0xABAB";
#else
            char msg[] = "Hello World from Sensor Node!";
            char addr[] = "Short Address: 0x0001";
#endif

            /* Pass to tx_buffer to MAC Transmission */
            myNode.write((const uint8_t *)msg, sizeof(msg));
            myNode.write((const uint8_t *)addr, sizeof(addr));


            /* Signal to 15.4 that we are ready to send */
            bool error = myNode.endTransmission(sendShortAddr);

            if(!error) {
                Serial.println("Failed to send msg");
            }

            start = getCurrentTime();
         }

        uint8_t available = myNode.available();
        if(available)
        {
            /* User creates a local buffer */
            char buf[100];

            /* Take data from our rx_buffer, user now has access to it */
            myNode.read((uint8_t *)buf, available);

            /* This prints "Hello, World!" to Serial console, until EOS terminated */
            Serial.println(buf);
        }

        /* Continue with MAC processing */
        myNode.process();

    }
}

int main(void)
{
    Task_Params taskParams;

    Board_init();

    Task_Params_init(&taskParams);
    taskParams.stack = appTaskStack;
    taskParams.stackSize = APP_TASK_STACK_SIZE;
    taskParams.priority = APP_TASK_PRIORITY;
    Task_construct(&appTask, appTaskFxn, &taskParams, NULL);

    BIOS_start();

    return (0);
}
