/*
 * asserts.c
 *
 *  Created on: Jun 4, 2020
 *      Author: robertinant
 */

#include <xdc/std.h>
#include <ti/sysbios/hal/Hwi.h>
#include "hal_types.h"
/* Assert Reasons */
#define MAIN_ASSERT_MAC          3
#define MAIN_ASSERT_HWI_TIRTOS   4

/*
 When assert happens, this field will be filled with the reason:
       MAIN_ASSERT_HWI_TIRTOS or MAIN_ASSERT_MAC
 */
uint8 Main_assertReason = 0;

/*!
 * @brief       Fill in your own assert function.
 *
 * @param       assertReason - reason: MAIN_ASSERT_HWI_TIRTOS or
 *                                     MAIN_ASSERT_MAC
 */
void Main_assertHandler(uint8_t assertReason)
{
    Main_assertReason = assertReason;

#if defined(RESET_ASSERT)
    Ssf_assertInd(assertReason);

    /* Pull the plug and start over */
    SysCtrlSystemReset();
#else
    Hwi_disable();
    while(1)
    {
        /* Put you code here to do something if in assert */
    }
#endif
}

/*!
 * @brief       TIRTOS HWI Handler.  The name of this function is set to
 *              M3Hwi.excHandlerFunc in app.cfg, you can disable this by
 *              setting it to null.
 *
 * @param       excStack - TIROS variable
 * @param       lr - TIROS variable
 */
xdc_Void Main_excHandler(UInt *excStack, UInt lr)
{
    /* User defined function */
    Main_assertHandler(MAIN_ASSERT_HWI_TIRTOS);
}

/*!
 * @brief       HAL assert handler required by OSAL memory module.
 */
void assertHandler(void)
{
    /* User defined function */
    Main_assertHandler(MAIN_ASSERT_MAC);
}
