#ifndef CC1111RF_H
#define CC1111RF_H

#include "cc1111.h"

// use DMA for RF?
//#define RFDMA

#define DMA_CFG_SIZE 8
// BUFFER size must match RF_MAX_RX_BLOCK defined in rflib/cc1111client.py 
#define BUFFER_SIZE 512
#define BUFFER_AMOUNT 2

#define PKTCTRL0_LENGTH_CONFIG_INF        (0x02)
#define RF_MAX_TX_BLOCK                   (u16) 255

#define RSSI_TIMEOUT_US 1500

#define RF_STATE_RX 1
#define RF_STATE_TX 2
#define RF_STATE_IDLE 3

#define RF_SUCCESS 0

#define RF_DMA_VLEN_1       1<<5
#define RF_DMA_VLEN_3       4<<5
#define RF_DMA_LEN          0xfe
#define RF_DMA_WORDSIZE16   1<<7
#define RF_DMA_WORDSIZE8    0<<7
#define RF_DMA_TMODE        0
#define RF_DMA_TRIGGER      19
#define RF_DMA_DST_INC      1<<4
#define RF_DMA_SRC_INC      1<<6
#define RF_DMA_IRQMASK_DI   0<<3
#define RF_DMA_IRQMASK_EN   1<<3
#define RF_DMA_M8           0<<2
#define RF_DMA_M7           1<<2
#define RF_DMA_PRIO_LOW     0<<1
#define RF_DMA_PRIO_NOR     1<<1
#define RF_DMA_PRIO_HIGH    1<<2

#define FIRST_BUFFER 0
#define SECOND_BUFFER 1
#define RX_UNPROCESSED 0
#define RX_PROCESSED 1

/* Type for registers:
    NORMAL: registers are configured by client
    RECV: registers are set for receive
    XMIT: registers are set for transmit
*/
typedef enum{NORMAL,RECV,XMIT} register_e;

/* Rx buffers */
extern volatile __xdata u8 rfRxCurrentBuffer;
extern volatile __xdata u8 rfrxbuf[BUFFER_AMOUNT][BUFFER_SIZE];
extern volatile __xdata u16 rfRxCounter[BUFFER_AMOUNT];
extern volatile __xdata u8 rfRxProcessed[BUFFER_AMOUNT];
extern volatile __xdata u8 rfRxInfMode;
extern volatile __xdata u16 rfRxTotalRXLen;
extern volatile __xdata u16 rfRxLargeLen;
/* Tx buffers */
extern volatile __xdata u8 *rftxbuf;
extern volatile __xdata u16 rfTxCounter;

extern volatile __xdata u16 rf_MAC_timer;
extern volatile __xdata u16 rf_tLastRecv;

// AES
extern volatile __xdata u8 rfAESMode;

extern u8 rfif;

void rfTxRxIntHandler(void) __interrupt RFTXRX_VECTOR; // interrupt handler should transmit or receive the next byte
void rfIntHandler(void) __interrupt RF_VECTOR; // interrupt handler should trigger on rf events

// set semi-permanent states
void RxMode(void);          // set defaults to return to RX and calls RFRX
void TxMode(void);          // set defaults to return to TX and calls RFTX
void IdleMode(void);        // set defaults to return to IDLE and calls RFOFF

// set transient RF mode (like.  NOW!)
// set RF mode to RX and wait until MARCSTATE shows it's there
#define RFTX RFST = RFST_STX; while ((MARCSTATE) != MARC_STATE_TX);
// set RF mode to TX and wait until MARCSTATE shows it's there
#define RFRX RFST = RFST_SRX; while ((MARCSTATE) != MARC_STATE_RX);
// set RF mode to IDLE and wait until MARCSTATE shows it's there
#define RFCAL RFST=RFST_SCAL; while ((MARCSTATE) != MARC_STATE_IDLE);
// set RF mode to CAL and wait until MARCSTATE shows it's done (in IDLE)
#define RFOFF RFST=RFST_SIDLE; while ((MARCSTATE) != MARC_STATE_IDLE);


int waitRSSI(void);

u8 transmit(__xdata u8*, u16 len, u16 repeat, u16 offset);   // sends data out the radio using the current RF settings
void appInitRf(void);       // in application.c  (provided by the application and called from init_RF()
void init_RF(void);
void byte_shuffle(__xdata u8* buf, u16 len, u16 offset);
void startRX(void);
#endif
