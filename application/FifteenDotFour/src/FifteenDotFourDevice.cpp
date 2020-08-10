#include <software_stack/ti15_4stack/macTask.h>
#include <software_stack/ti15_4stack/mac/rom/rom_jt_154.h>
#include <advanced_config.h>
#include <FifteenDotFourDevice.h>
#include <xdc/runtime/System.h>
#include "ti_154stack_config.h"
#include <mac_util.h>


#define DEFAULT_KEY_SOURCE {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33}

extern "C" ApiMac_sAddrExt_t ApiMac_extAddr;
static uint8_t defaultChannelMask[APIMAC_154G_CHANNEL_BITMAP_SIZ] = CONFIG_CHANNEL_MASK;

extern "C" void assertHandler(void);

extern "C" {
static ApiMac_callbacks_t Sensor_macCallbacks =
    {
      /*! Associate Indicated callback */
      NULL,
      /*! Associate Confirmation callback */
      FifteenDotFourDevice::associateCnfCB,
      /*! Disassociate Indication callback */
      NULL,
      /*! Disassociate Confirmation callback */
      NULL,
      /*! Beacon Notify Indication callback */
      FifteenDotFourDevice::beaconNotifyIndCb,
      /*! Orphan Indication callback */
      NULL,
      /*! Scan Confirmation callback */
      FifteenDotFourDevice::scanCnfCB,
      /*! Start Confirmation callback */
      NULL,
      /*! Sync Loss Indication callback */
      NULL,
      /*! Poll Confirm callback */
      FifteenDotFourDevice::pollCnfCb,
      /*! Comm Status Indication callback */
      NULL,
      /*! Poll Indication Callback */
      NULL,
      /*! Data Confirmation callback */
      FifteenDotFourDevice::dataCnfCB,
      /*! Data Indication callback */
      FifteenDotFourDevice::dataIndCB,
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

FifteenDotFourDevice::FifteenDotFourDevice() : FifteenDotFour(true)
{
    /*
     * Assign global class ptr to this instance.
     * We need a pointer to the class instance to be able
     * to access the class properties in the C callbacks.
     */
    _this = this;
}

/*--------------------------------------------------------------------*/
/* These methods should be aggregated into FifteenDotFour parent class once
 * we figure out what can be consolidated
 */
bool FifteenDotFourDevice::beginTransmission(uint16_t address)
{
    // set the address of the destination node
//    setAddressExt())
    // clear buffer
    flush();
// creaet tx_buffer and rx_buffer
//    buffer_init(&tx_buffer);
    return true;
}

bool FifteenDotFourDevice::endTransmission(uint16_t address)
{
   ApiMac_mcpsDataReq_t dataReq;
   memset(&dataReq, 0, sizeof(ApiMac_mcpsDataReq_t));

   dataReq.dstAddr.addrMode = ApiMac_addrType_short;
   dataReq.dstAddr.addr.shortAddr = address;      /* hard coded shortAddr of 0x0001 device */
   dataReq.srcAddrMode = ApiMac_addrType_short;
   dataReq.dstPanId = 0xfafa;
   dataReq.msduHandle = 0;
   dataReq.txOptions.ack = true;
   dataReq.txOptions.indirect = false;            /* True if polling device */

   /*-------------------------------------------------------*/
   /* Buffer Handling */
   /*-------------------------------------------------------*/
   uint8_t msgSize = buffer_get_size(&tx_buffer);
//   memset(dataReq.msdu.p, 0, msgSize);
   dataReq.msdu.len = msgSize;
//   dataReq.msdu.p = (uint8_t*)malloc(msgSize);                  // give ptr a valid mem address
//   memset(dataReq.msdu.p, 0, msgSize);
//   buffer_read_multiple(dataReq.msdu.p, &tx_buffer, msgSize);
   dataReq.msdu.p = tx_buffer.buffer;
   /*-------------------------------------------------------*/

   ApiMac_status_t status = ApiMac_mcpsDataReq(&dataReq);

   // set last error to ApiMac_status
   return status == ApiMac_status_success ? true : false;
}
/*--------------------------------------------------------------------*/

void FifteenDotFourDevice::begin(bool autoJoin)
{
    /* Begin of parent class -> init buffers here or in constructor?*/
//    FifteenDotFour::begin();

    uint8_t _macTaskId;
    macUserCfg_t macUser0Cfg[] = MAC_USER_CFG;

    macUser0Cfg[0].pAssertFP = assertHandler;
    macUser0Cfg[0].ff = false;

    Task_disable();
    _macTaskId = macTaskInit(macUser0Cfg);
    Task_enable();

    initializePollClock();
    initializeScanClock();

    if(autoJoin) {
        this->revents |= JOIN_EVENT;
    }

    /* Get the Primary IEEE Address */
    memcpy(ApiMac_extAddr, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET), (APIMAC_SADDR_EXT_LEN));

    /* Initialize MAC without frequency hopping */
    sem = (Semaphore_Handle)ApiMac_init(_macTaskId, false);

    ApiMac_registerCallbacks(&Sensor_macCallbacks);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_phyCurrentDescriptorId,
                           (uint8_t)CONFIG_PHY_ID);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_channelPage,
                           (uint8_t)CONFIG_CHANNEL_PAGE);
    /* Set the transmit power */
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
}

void FifteenDotFourDevice::connect(void)
{
    revents |= JOIN_EVENT;
}

void FifteenDotFourDevice::process(void)
{
    if(revents & JOIN_EVENT) {
        revents &= ~JOIN_EVENT;

        ApiMac_mlmeScanReq_t scanReq;

        /*Turn receiver ON for SCAN */
        ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, true);

        /* set common parameters for all scans */
        memset(&scanReq, 0, sizeof(ApiMac_mlmeScanReq_t));
        /* set scan channels from channel mask*/
        memcpy(scanReq.scanChannels, defaultChannelMask,
               APIMAC_154G_CHANNEL_BITMAP_SIZ);
        scanReq.scanType = ApiMac_scantype_active;
        if(CONFIG_MAC_BEACON_ORDER == JDLLC_BEACON_ORDER_NON_BEACON)
        {
            scanReq.scanDuration = CONFIG_SCAN_DURATION;
        } else {
            scanReq.scanDuration = CONFIG_MAC_BEACON_ORDER;
        }

        scanReq.maxResults = 0;/* Expecting beacon notifications */
        scanReq.permitJoining = false;
        scanReq.linkQuality = CONFIG_LINKQUALITY;
        scanReq.percentFilter = CONFIG_PERCENTFILTER;
        scanReq.channelPage = CONFIG_CHANNEL_PAGE;
        scanReq.phyID = CONFIG_PHY_ID;
        /* using no security for scan request command */
        memset(&scanReq.sec, 0, sizeof(ApiMac_sec_t));
        /* send scan Req */
        ApiMac_mlmeScanReq(&scanReq);
    }

    if(revents & ACCOCIATE_EVENT) {
        revents &= ~ACCOCIATE_EVENT;
        ApiMac_mlmeAssociateReq_t assocReq;
        memset(&assocReq, 0, sizeof(ApiMac_mlmeAssociateReq_t));
        assocReq.coordPanId = getPanID();
        assocReq.logicalChannel = getChannel();
        assocReq.coordAddress.addrMode = ApiMac_addrType_short;
        assocReq.coordAddress.addr.shortAddr = getCoordShortAddr();
        assocReq.channelPage = CONFIG_CHANNEL_PAGE;
        assocReq.phyID = CONFIG_PHY_ID;
        assocReq.sec.securityLevel = ApiMac_secLevel_none;
        assocReq.capabilityInformation.allocAddr = true;
        assocReq.capabilityInformation.ffd = false;
        assocReq.capabilityInformation.panCoord = false;
        assocReq.capabilityInformation.rxOnWhenIdle = false;
        ApiMac_mlmeAssociateReq(&assocReq);
    }

    if(revents & IDLE_EVENT) {
        revents &= ~IDLE_EVENT;
        ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, false);
        if(connected()) {
            /* Start polling timer to poll collector for data */
            setPollClock(POLLING_INTERVAL);
        } else {
            setScanClock(SCAN_INTERVAL);
        }
    }

    if(revents & POLL_EVENT) {
        revents &= ~POLL_EVENT;
        revents |= IDLE_EVENT;
        ApiMac_mlmePollReq_t pollReq;

        memset(&pollReq, 0, sizeof(ApiMac_mlmePollReq_t));
        pollReq.coordPanId = getPanID();
        pollReq.coordAddress.addrMode = ApiMac_addrType_short;
        pollReq.coordAddress.addr.shortAddr = getCoordShortAddr();
        ApiMac_mlmePollReq(&pollReq);
    }

    if(revents == 0) {
        ApiMac_processIncoming();
    }
}

void FifteenDotFourDevice::pollCnfCb(ApiMac_mlmePollCnf_t *pData) {
    if((pData->status == ApiMac_status_noData) || (pData->status == ApiMac_status_success)) {
        _this->revents = IDLE_EVENT;
    }
}
/*
 * MAC API C callbacks.
 */
void FifteenDotFourDevice::dataCnfCB(ApiMac_mcpsDataCnf_t *pDataCnf)
{
    // data sending state ->
        //    if (CONFIC_MAC_BEACON_ORDER == JDLLC_BEACON_ORDER_NON_BEACON)
        //    {
                if(pDataCnf->status == ApiMac_status_noAck)
                {
                    //_this->updateDataFailures()
                    // how to handle max data failures or not -> switch to a polling state
                    _this->revents = POLL_EVENT;
                }
                else if (pDataCnf->status == ApiMac_status_success)
                {
                    //-this->resetDataFailures
                    _this->revents = IDLE_EVENT;
                }
        //    }
}

/*!
 * @brief      MAC Data Indication callback.
 *
 * @param      pDataInd - pointer to the data indication information
 */

void FifteenDotFourDevice::dataIndCB(ApiMac_mcpsDataInd_t *pDataInd)
{
    if(pDataInd != NULL && pDataInd->msdu.p != NULL && pDataInd->msdu.len > 0)
    {
        if(pDataInd->dstPanId == _this->getPanID())
        {

          buffer_write_multiple(&_this->rx_buffer, pDataInd->msdu.p, (size_t)pDataInd->msdu.len);
        }
    }
}

void FifteenDotFourDevice::beaconNotifyIndCb(ApiMac_mlmeBeaconNotifyInd_t *pData)
{
    if(pData->beaconType == ApiMac_beaconType_normal) {
        if(APIMAC_SFS_ASSOCIATION_PERMIT(pData->panDesc.superframeSpec))
        {
            /* Check for beacon order match */
            if(_this->checkBeaconOrder(pData->panDesc.superframeSpec) == true)
            {
                /* Sensor with 0xFFFF as panID can join any network */
                if(0xFFFF == _this->getPanID())
                {
                    _this->setPanID(pData->panDesc.coordPanId);
                    _this->setPanIdMatch(true);
                }

                if (pData->panDesc.coordPanId == _this->getPanID())
                {
                    ApiMac_mlmeSetReqBool(ApiMac_attribute_autoRequest, true);
                    _this->setCoordShortAddr(pData->panDesc.coordAddress.addr.shortAddr);
                    _this->setChannel(pData->panDesc.logicalChannel);
                    _this->setPanIdMatch(true);
                }
            }
        }
    }
 }

void FifteenDotFourDevice::scanCnfCB(ApiMac_mlmeScanCnf_t *pData)
{
    if(pData->status == ApiMac_status_success) {
        if(_this->getPanIdMatch()) {
            /* go to the association state */
            _this->revents |= ACCOCIATE_EVENT;
//            if(Timer_isActive(&_this->scanClkStruct) == true)
//            {
//                Timer_stop(&_this->scanClkStruct);
//            }
        }
    } else {
        _this->revents |= IDLE_EVENT;
        if(Timer_isActive(&_this->scanClkStruct) == true)
        {
            Timer_stop(&_this->scanClkStruct);
        }

    }
}

void FifteenDotFourDevice::associateCnfCB(ApiMac_mlmeAssociateCnf_t *pAssocCnf)
{
    _this->revents |= IDLE_EVENT;
    _this->_connected = true;
}

bool FifteenDotFourDevice::checkBeaconOrder(uint16_t superframeSpec)
{
    if(CONFIG_MAC_BEACON_ORDER == JDLLC_BEACON_ORDER_NON_BEACON)
    {
        if(APIMAC_SFS_BEACON_ORDER(superframeSpec) == CONFIG_MAC_BEACON_ORDER)
        {
            return (true);
        }
    }
    else if((APIMAC_SFS_BEACON_ORDER(superframeSpec) <= CONFIG_MAC_BEACON_ORDER))
    {
        return (true);
    }

    return (false);
}

/*
 * Scan timer related functions
 */
void FifteenDotFourDevice::initializeScanClock(void)
{
    scanClkHandle = Timer_construct(&scanClkStruct,
                                         processScanTimeoutCallback,
                                         SCAN_TIMEOUT_VALUE,
                                         0,
                                         false,
                                         (UArg)this);
}

void FifteenDotFourDevice::setScanClock(uint32_t scanTime)
{
    /* Stop the Reading timer */
    if(Timer_isActive(&scanClkStruct) == true)
    {
        Timer_stop(&scanClkStruct);
    }

    /* Setup timer */
    if(scanTime > 0)
    {
        Timer_setTimeout(scanClkHandle, scanTime);
        Timer_start(&scanClkStruct);
    }
}

void FifteenDotFourDevice::processScanTimeoutCallback(UArg instance)
{
    FifteenDotFourDevice *node = static_cast<FifteenDotFourDevice*>((void *)instance);

    /* Wake up the application thread when it waits for clock event */
    node->revents |= JOIN_EVENT;
    Semaphore_post(node->sem);
}

/*
 * Data poll timer related functions
 */
void FifteenDotFourDevice::initializePollClock(void)
{
    pollClkHandle = Timer_construct(&pollClkStruct,
                                         processPollTimeoutCallback,
                                         POLL_TIMEOUT_VALUE,
                                         0,
                                         false,
                                         (UArg)this);
}

void FifteenDotFourDevice::setPollClock(uint32_t pollTime)
{
    /* Stop the Reading timer */
    if(Timer_isActive(&pollClkStruct) == true)
    {
        Timer_stop(&pollClkStruct);
    }

    /* Setup timer */
    if(pollTime > 0)
    {
        Timer_setTimeout(pollClkHandle, pollTime);
        Timer_start(&pollClkStruct);
    }
}

void FifteenDotFourDevice::processPollTimeoutCallback(UArg instance)
{
    FifteenDotFourDevice *node = static_cast<FifteenDotFourDevice*>((void *)instance);

    /* Wake up the application thread when it waits for clock event */

    node->revents |= POLL_EVENT;
    Semaphore_post(node->sem);
}
