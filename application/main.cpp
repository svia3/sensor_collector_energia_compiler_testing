#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
//#include "ti_drivers_config.h"
//#include "macTask.h"
////#include <application/FifteenDotFour/src/FifteenDotFourCollector.h>
////#include <FifteenDotFourDevice.h>
//#include <inc/hw_ccfg.h>
//#include <inc/hw_ccfg_simple_struct.h>
//
//#include "mac_user_config.h"
#include "macconfig.h"
#include "nvocmp.h"
//#include <Energia.h>
//#include <ti/runtime/wiring/cc13x2/variants/CC1312R1_LAUNCHXL/pins_energia.h>
//#include <WiFi.h>
//#include <stdio.h>

#define EXTADDR_OFFSET 0x2F0

extern void loop_collector();
extern void loop_sensor();
extern void setup_collector();
extern void setup_sensor();

#define APP_TASK_PRIORITY   1
#define APP_TASK_STACK_SIZE 2048

extern ApiMac_sAddrExt_t ApiMac_extAddr;
Task_Struct appTask;
static uint8_t appTaskStack[APP_TASK_STACK_SIZE];

//static uint8_t _macTaskId;
#ifdef NV_RESTORE
mac_Config_t Main_user1Cfg =
    { 0 };
#endif

#define COLLECTOR

extern "C" void Board_init(void);

void appTaskFxn(UArg a0, UArg a1)
{

    /* Get the baked-in primary IEEE Address */
    memcpy(ApiMac_extAddr, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET),
               (APIMAC_SADDR_EXT_LEN));

#ifdef COLLECTOR
    /* 15.4 collector setup */
    setup_collector();
#else
    /* 15.4 sensor setup */
    setup_sensor();
#endif

    while(1)
    {
#ifdef COLLECTOR
        /* 15.4 collector loop */
        loop_collector();
#else
        /* 15.4 sensor loop */
        loop_sensor();
#endif
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
