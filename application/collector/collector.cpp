#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "ti_drivers_config.h"
#include "macTask.h"
#include <FifteenDotFourCollector.h>
//#include <application/FifteenDotFour/src/FifteenDotFourDevice.h>
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
uint32_t collectorCurrentTime();
void setup_collector();
void loop_collector();
void printWifiData();
void printCurrentNet();
void startWifi(const char* ssid, const char* pwd);

/* -------------------------- */
/* Create global 15.4 instance*/
/* -------------------------- */
FifteenDotFourCollector Collector;

uint32_t collectorCurrentTime()
{
    return Clock_getTicks() / (1000 / Clock_tickPeriod);
}

void setup_collector()
{

    Serial.begin(115200);
    Serial.println("Starting . . .");

    /* Connect to local Network using Energia Framework */
    const char ssid[] = "FiOS-XY4PM_EXT";
    const char pwd[] = "italy123";

    startWifi(ssid, pwd);
    printCurrentNet();
    printWifiData();

    /* Create macTask and semaphore */
    Collector.begin();

    /* Configure to join any network by setting pan id to 0xFFFF. */
    uint16_t CPANID = 0xFAFA;
    Collector.setPanID(CPANID);
    /* Connect to any network allowing join */
    Collector.start();
}


void loop_collector()
{

    /* Kick off application - Forever loop */
    static uint32_t now = 0, then = 0;
    now = collectorCurrentTime();

    /* ------------------------------------ */
    /* Process data request every 5 seconds */
    /* ------------------------------------ */
    if((now - then) > 5000)
    {

        /* Flush our tx_buffer; geting ready to send */
        Collector.beginTransmission(1);

        /* Create some data */
        uint8_t msg[] = "Hello World from Collector Node!\n";
        uint8_t addr[] = "Short Address: 0xABAB\0";
//        char msg[] = "Hello World from Sensor Node!";
//        char addr[] = "Short Address: 0x0001";

        /* Pass to tx_buffer to MAC Transmission */
        Collector.write((const uint8_t *)msg, sizeof(msg));
        Collector.write((const uint8_t *)addr, sizeof(addr));


        /* Signal to 15.4 that we are ready to send -> to sensor */
        uint16_t destShortAddr = 0x0001;
        bool error = Collector.endTransmission(destShortAddr);

        if(!error)
        {
            Serial.println("Failed to send msg");
        }

        then = collectorCurrentTime();
    }

    /* -------------------------- */
    /* Check for data in rx_buffer*/
    /* -------------------------- */
    uint8_t available = Collector.available();
    if(available)
    {
        /* User creates a local buffer */
        char buf[100];

        /* Take data from our rx_buffer, user now has access to it */
        Collector.read((uint8_t *)buf, available);

        /* This prints "Hello, World!" to Serial console, until EOS terminated */
        Serial.println(buf);
    }

    /* Continue with MAC processing */
    Collector.process();
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


//void printFifteenDotFourStats()
//{
//    /* Printing the PAN ID */
//    char msg[50];
//    sprintf(msg, "Pan ID: %02X", Collector.getPanID());
//    Serial.println(msg);
//
//#ifdef COLLECTOR
//    // do nothing for now
//#else
//    /* Connecting  */
//    Serial.println("Connecting to Collector ...");
//    if (Collector.connected()) Serial.println("Connected!");
//        /* ----------- */
//    /* Printing the Coordinator Short Address */
//    sprintf(msg, "Coordinator Short Address: %02X", Collector.getCoordShortAddr());
//    Serial.println(msg);
//#endif
//}
