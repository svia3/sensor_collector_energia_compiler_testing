#ifndef FifteenDotFourDevice_h
#define FifteenDotFourDevice_h

#include <stdint.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <software_stack/ti15_4stack/radio_configuration/mac_user_config.h>
#include <utils/util_timer.h>
#include <FifteenDotFour.h>

/*
 * TODO: Some of these come from jdllc.c.
 * - Should they be moved to elsewhere?
 * - Should they be configurable?
 */
#define EXTADDR_OFFSET 0x2F0
#define JDLLC_BEACON_ORDER_NON_BEACON   15
#define CONFIG_LINKQUALITY              1
#define CONFIG_PERCENTFILTER            0xFF
#define POLL_TIMEOUT_VALUE              30000
#define SCAN_TIMEOUT_VALUE              30000
#define POLLING_INTERVAL                2000
#define SCAN_INTERVAL                   2000

#define JOIN_EVENT (1 << 0)
#define ACCOCIATE_EVENT (1 << 1)
#define IDLE_EVENT (1 << 2)
#define POLL_EVENT (1 << 3)

/******************************************************************************
 Security constants and definitions
 *****************************************************************************/

#define KEY_TABLE_ENTRIES 1
#define KEY_ID_LOOKUP_ENTRIES 1
#define KEY_DEVICE_TABLE_ENTRIES 8
#define KEY_USAGE_TABLE_ENTRIES 1
#define SECURITY_LEVEL_ENTRIES 1

#define MAC_FRAME_TYPE_DATA 1
#define MAC_DATA_REQ_FRAME 4

#define AUTO_REQUEST_SEC_LEVEL 0x00

class FifteenDotFourDevice : public FifteenDotFour
{
	public:
		FifteenDotFourDevice();
		void begin(bool autoJoin = false);
		void process(void);
		/* TODO: provide callback option vs polling connected()? */
		void connect(void);
		bool connected(void) {return _connected;};
		void setChannel(uint8_t c) {channel = c;};
		uint8_t getChannel(void) {return channel;};
		void setPanID(uint16_t id) {panID = id;};
		uint16_t getPanID(void) {return panID;};
		void setPanIdMatch(bool match) {panIdMatch = match;};
		bool getPanIdMatch(void) {return panIdMatch;};
		void setCoordShortAddr(uint16_t addr) {coordShortAddr = addr;};
		uint16_t getCoordShortAddr(void) {return coordShortAddr;};

        /* ------------------------------------------------- */
        /*     Static Callback Functions for the MAC         */
        /* ------------------------------------------------- */
		static void dataCnfCB(ApiMac_mcpsDataCnf_t *pDataCnf);
		static void dataIndCB(ApiMac_mcpsDataInd_t *pDataInd);
		static void scanCnfCB(ApiMac_mlmeScanCnf_t *pScanCnf);
		static void beaconNotifyIndCb(ApiMac_mlmeBeaconNotifyInd_t *pData);
		static void associateCnfCB(ApiMac_mlmeAssociateCnf_t *pAssocCnf);
		static void pollCnfCb(ApiMac_mlmePollCnf_t *pData);
		static void processPollTimeoutCallback(UArg instance);
		static void processScanTimeoutCallback(UArg instance);
        /* ------------------------------------------------- */


	private:
		/*
		 * Private Methods
		 */
		void initializePollClock(void);
		void setPollClock(uint32_t pollTime);
        void initializeScanClock(void);
        void setScanClock(uint32_t pollTime);

        /*
         * Bookkeeping Variables
         */
		bool checkBeaconOrder(uint16_t superframeSpec);
		bool fh = false;
		uint8_t channel = 0;
		uint16_t panID = 0;
        uint16_t revents = 0;
		uint16_t states = 0;
		uint8_t _macTaskId = 0;
		bool panIdMatch = false;
		uint16_t coordShortAddr = 0;
		bool _connected = false;
		Clock_Struct pollClkStruct;
		Clock_Handle pollClkHandle;
        Clock_Struct scanClkStruct;
        Clock_Handle scanClkHandle;

        /* Buffer Handling implementing buffer_c API */
        buffer_t rx_buffer;
        buffer_t tx_buffer;

	protected:
		macUserCfg_t macUser0Cfg[];
		Semaphore_Handle sem;
};

#endif
