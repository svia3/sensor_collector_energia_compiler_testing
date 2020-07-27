#ifndef ADVANCED_H
#define ADVANCED_H

/*! MAC MAX CSMA Backoffs */
#define CONFIG_MAC_MAX_CSMA_BACKOFFS 5

/* Number of max data retries */
#define CONFIG_MAX_RETRIES 5


/*! Setting for maximum devices in association table */
#define CONFIG_MAX_DEVICES                  50
/* Starting counter */
#define ASSOC_DEVICE_STARTING_SHORT_ADDR    1
/* Invalid short address */
#define CSF_INVALID_SHORT_ADDR              0xFFFF
/*! Association status */
#define ASSOC_STATUS_DEAD  (1 << 0)
#define ASSOC_STATUS_ALIVE (1 << 1)


/*! Building block for association table */
typedef struct associationDevice_t
{
    /*! Extended addr */
    ApiMac_sAddrExt_t extAddress;
    /*! Short address of associated device, use this as key for lookup*/
    uint16_t shortAddress;
    /*! capability information */
    ApiMac_capabilityInfo_t capInfo;
    /*! RSSI */
    int8_t rssi;
    /*! Device alive status */
    uint8_t status;
} associationDevice_t;

///* Hash Table Implementation */
//typedef struct associationTable_t
//{
//    /* pointer to start of the table */
//    associationDevice_t**    items;
//    /* number of devices in the table */
//    uint8_t size;
//    /* capacity */
////    uint8_t capacity;
//} associationTable_t;

#endif /* ADVANCED_H */


