#include <xdc/runtime/System.h>
#include "ti_154stack_config.h"
#include <advanced_config.h>
#include <FifteenDotFourCollector.h>
#include <software_stack/ti15_4stack/macTask.h>
#include <software_stack/ti15_4stack/mac/rom/rom_jt_154.h>
#include <mac_util.h>
//#include <utils/buffer_c/buffer.h>

extern "C" void assertHandler(void);

extern "C" {
/*! API MAC Callback table */
ApiMac_callbacks_t Collector_macCallbacks =
    {
      /*! Associate Indicated callback */
     FifteenDotFourCollector::assocIndCb,
      /*! Associate Confirmation callback */
      NULL,
      /*! Disassociate Indication callback */
      FifteenDotFourCollector::disassocIndCb,
      /*! Disassociate Confirmation callback */
      FifteenDotFourCollector::disassocCnfCb,
      /*! Beacon Notify Indication callback */
      FifteenDotFourCollector::beaconNotifyIndCb,
      /*! Orphan Indication callback */
      FifteenDotFourCollector::orphanIndCb,
      /*! Scan Confirmation callback */
      FifteenDotFourCollector::scanCnfCb,
      /*! Start Confirmation callback */
      FifteenDotFourCollector::startCnfCb,
      /*! Sync Loss Indication callback */
      NULL,
      /*! Poll Confirm callback */
      NULL,
      /*! Comm Status Indication callback */
      FifteenDotFourCollector::commStatusIndCB,
      /*! Poll Indication Callback */
      FifteenDotFourCollector::pollIndCB,
      /*! Data Confirmation callback */
      FifteenDotFourCollector::dataCnfCB,
      /*! Data Indication callback */
      FifteenDotFourCollector::dataIndCB,
      /*! Purge Confirm callback */
      NULL,
      /*! WiSUN Async Indication callback */
      NULL,
      /*! WiSUN Async Confirmation callback */
      NULL,
      /*! Unprocessed message callback */
      NULL
    };
}

FifteenDotFourCollector *_this;

FifteenDotFourCollector::FifteenDotFourCollector(void) : FifteenDotFour(false)
{
    /*
     * Assign global class ptr to this instance.
     * We need a pointer to the class instance to be able
     * to access the class properties in the C callbacks.
     */
    _this = this;
    panID = 0x0001;
//    FifteenDotFour::FifteenDotFour(false);
}

void FifteenDotFourCollector::begin(void)
{

    /* Initialize the buffers for the device */
//    FifteenDotFour::begin();

    uint8_t _macTaskId;
    macUserCfg_t macUser0Cfg[] = MAC_USER_CFG;

    macUser0Cfg[0].pAssertFP = assertHandler;
    macUser0Cfg[0].ff = true;

    /*
     * Tasks have to be disabled before calling macTaskInit()
     * To prevent it from running before returning here.
     */

    Task_disable();
    _macTaskId = macTaskInit(macUser0Cfg);
    Task_enable();

    /* Initialize the MAC */
    sem = (Semaphore_Handle)ApiMac_init(_macTaskId, false);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_phyCurrentDescriptorId,
                           (uint8_t)CONFIG_PHY_ID);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_channelPage,
                           (uint8_t)CONFIG_CHANNEL_PAGE);

    /* Register the MAC Callbacks */
    ApiMac_registerCallbacks(&Collector_macCallbacks);


    ApiMac_mlmeSetReqUint16(ApiMac_attribute_transactionPersistenceTime,
                            INDIRECT_PERSISTENT_TIME);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_phyTransmitPowerSigned,
                           (uint8_t)CONFIG_TRANSMIT_POWER);

    /* Set Min BE */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_backoffExponent,
                              (uint8_t)CONFIG_MIN_BE);
    /* Set Max BE */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_maxBackoffExponent,
                              (uint8_t)CONFIG_MAX_BE);
    /* Set MAC MAX CSMA Backoffs */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_maxCsmaBackoffs,
                              (uint8_t)CONFIG_MAC_MAX_CSMA_BACKOFFS);
    /* Set MAC MAX Frame Retries */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_maxFrameRetries,
                              (uint8_t)CONFIG_MAX_RETRIES);

    ApiMac_mlmeSetReqUint16(ApiMac_attribute_shortAddress,
                            0xABAB);
    ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle,true);
}

void FifteenDotFourCollector::start(void)
{
    revents |= START_EVENT;
}

void FifteenDotFourCollector::process(void)
{
    if(revents & START_EVENT) {
        ApiMac_status_t status;

        revents &= ~START_EVENT;
        ApiMac_mlmeStartReq_t startReq;
        memset(&startReq, 0, sizeof(ApiMac_mlmeStartReq_t));

        /* read network parameters fill them in start req */
        startReq.startTime = 0;
        startReq.panId =  0xFAFA;
        startReq.logicalChannel = channel;
        startReq.channelPage = CONFIG_CHANNEL_PAGE;
        startReq.phyID = CONFIG_PHY_ID;
        startReq.beaconOrder = CONFIG_MAC_BEACON_ORDER;
        startReq.superframeOrder = CONFIG_MAC_SUPERFRAME_ORDER;
        startReq.panCoordinator = true;
        startReq.batteryLifeExt = false;
        startReq.coordRealignment = false;
        startReq.realignSec.securityLevel = false;
        startReq.startFH = false;
        startReq.mpmParams.offsetTimeSlot = CLLC_OFFSET_TIMESLOT;
        startReq.mpmParams.eBeaconOrder = CLLC_EBEACONORDER;
        startReq.mpmParams.NBPANEBeaconOrder = CLLC_NBPANEBEACONORDER;
        startReq.mpmParams.pIEIDs = NULL;

        /* send Start Req to MAC API */
        status = ApiMac_mlmeStartReq(&startReq);
    }

    if(revents & OPEN_EVENT)
    {
        revents &= ~OPEN_EVENT;
        ApiMac_mlmeSetReqBool(ApiMac_attribute_associatePermit, true);
    }

    if(revents == 0) {
        ApiMac_processIncoming();
    }
}

bool FifteenDotFourCollector::beginTransmission(uint16_t address)
{
    // set the address of the destination node
//    setAddressExt())
    // clear buffer
    FifteenDotFour::flush();
// creaet tx_buffer and rx_buffer
//    buffer_init(&tx_buffer);
    return true;
}

bool FifteenDotFourCollector::endTransmission()
{
    ApiMac_mcpsDataReq_t dataReq;
    memset(&dataReq, 0, sizeof(ApiMac_mcpsDataReq_t));

    dataReq.dstAddr.addrMode = ApiMac_addrType_short;
    dataReq.dstAddr.addr.shortAddr = 0x2000;
    dataReq.srcAddrMode = ApiMac_addrType_short;
    dataReq.dstPanId = 0xfafa;
    dataReq.msduHandle = 0;
    dataReq.txOptions.ack = true;
    dataReq.txOptions.indirect = true;

    /* Buffer Handling */
    int msgSize = buffer_get_size(&tx_buffer);
    dataReq.msdu.len = msgSize;
    buffer_read_multiple(dataReq.msdu.p, &tx_buffer, msgSize);

    ApiMac_status_t status = ApiMac_mcpsDataReq(&dataReq);

    // set last error to ApiMac_status
    return status == ApiMac_status_success ? true : false;
}

/* API MAC Callbacks */
void FifteenDotFourCollector::orphanIndCb(ApiMac_mlmeOrphanInd_t *pData)
{
}

void FifteenDotFourCollector::assocIndCb(ApiMac_mlmeAssociateInd_t *pData)
{
    ApiMac_mlmeAssociateRsp_t assocRsp;
    ApiMac_deviceDescriptor_t devInfo;

    memcpy(&assocRsp.deviceAddress, &pData->deviceAddress, 8);
    /* No security. Set to all 0's */
    memset(&assocRsp.sec, 0, sizeof(ApiMac_sec_t));
    assocRsp.status = ApiMac_assocStatus_success;
    memcpy(&assocRsp.deviceAddress, &devInfo.shortAddress, 16);

    /* Send out the associate response */
    ApiMac_mlmeAssociateRsp(&assocRsp);

}

void FifteenDotFourCollector::startCnfCb(ApiMac_mlmeStartCnf_t *pData)
{
    _this->revents |= OPEN_EVENT;
}

void FifteenDotFourCollector::disassocIndCb(ApiMac_mlmeDisassociateInd_t *pData)
{
}

void FifteenDotFourCollector::disassocCnfCb(ApiMac_mlmeDisassociateCnf_t *pData)
{
}

void FifteenDotFourCollector::commStatusIndCB(ApiMac_mlmeCommStatusInd_t *pCommStatusInd)
{
}

void FifteenDotFourCollector::pollIndCB(ApiMac_mlmePollInd_t *pPollInd)
{
}

void FifteenDotFourCollector::dataCnfCB(ApiMac_mcpsDataCnf_t *pDataCnf)
{
}

void FifteenDotFourCollector::dataIndCB(ApiMac_mcpsDataInd_t *pDataInd)
{
}

void FifteenDotFourCollector::beaconNotifyIndCb(ApiMac_mlmeBeaconNotifyInd_t *pData)
{
}

void FifteenDotFourCollector::scanCnfCb(ApiMac_mlmeScanCnf_t *pData)
{
}
