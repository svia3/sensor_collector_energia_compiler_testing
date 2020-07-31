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
#define CPANID 0X0001

#define COLLECTOR

#ifdef COLLECTOR
FifteenDotFourCollector myNode;
#else
FifteenDotFourDevice myNode;
#endif

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

void appTaskFxn(UArg a0, UArg a1)
{
    uint32_t now;
    uint32_t then;
    const char ssid[] = "energia";
    const char password[] = "launchpad";

    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);
    Serial.begin(115200);
    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to Network named: ");
    // print the network name (SSID);
    Serial.println(ssid);

    WiFi.begin((char *)ssid, (char *)password);

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

    printCurrentNet();
    printWifiData();

    /* Get the baked-in primary IEEE Address */
    memcpy(ApiMac_extAddr, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET),
               (APIMAC_SADDR_EXT_LEN));

    myNode.begin();

#ifdef COLLECTOR
    /* Configure to join any network by setting pan id to 0xFFFF. */
    myNode.setPanID(CPANID);
    /* Connect to any network allowing join */
    myNode.start();
#else
    myNode.setPanID(SPANID);
    myNode.connect();
    while(!myNode.connected()) {
        myNode.process();
        Task_sleep(100);
    }
#endif
    /* Kick off application - Forever loop */

    char msg[] = "Hello, World!";
//        uint32_t now, start;
    start = Clock_getTicks() * (1000 / Clock_tickPeriod);
    while(1)
    {
#ifdef COLLECTOR
        myNode.beginTransmission(1);
        // User does stuff//
        char msg[] = "Hello, World!";
//        char msg = 'a';
        myNode.write((const uint8_t*)msg, strlen(msg));
        // wrapper for end tranmission so dont flood mac
        now = Clock_getTicks() * (1000 / Clock_tickPeriod);
        if((now - start) > 5000) {
            bool error = myNode.endTransmission();
            start = Clock_getTicks() * (1000 / Clock_tickPeriod);
         }
#endif
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
