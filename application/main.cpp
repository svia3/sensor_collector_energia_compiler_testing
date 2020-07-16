#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "ti_drivers_config.h"
#include "macTask.h"
#include <application/FifteenDotFour/src/FifteenDotFourCollector.h>
#include <FifteenDotFourDevice.h>
#include <inc/hw_ccfg.h>
#include <inc/hw_ccfg_simple_struct.h>

//#include "mac_user_config.h"
#include "macconfig.h"
#include "nvocmp.h"

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

#define COLLECTOR 1

#ifdef COLLECTOR
FifteenDotFourCollector myNode;
#else
FifteenDotFour myNode;
#endif

void appTaskFxn(UArg a0, UArg a1)
{
    uint32_t now;
    uint32_t then;

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

    while(1)
    {
#ifdef COLLECTOR
        myNode.beginTransmission(1);
        // User does stuff//
        char msg[] = "Hello, World!";
//        char msg = 'a';
        myNode.write((const uint8_t*)msg, strlen(msg));
        bool error = myNode.endTransmission();
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

