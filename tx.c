
#include "contiki.h"
#include "cc2520.h"
#include <stdio.h> /* For printf() */
#include "net/netstack.h"
#include <stdint.h>
#include<string.h>
#include "dev/leds.h"

#define CHANNEL			15
#define PAN_ID			0xCDAC
#define SRC_NWK_ADDR		0x007A
#define DEST_NWK_ADDR		0x007B
#define MAX_PAYLOAD_SIZE	80
#define BASIC_MAC_HEADER_SIZE	9
uint8_t ieee_addr[] = {0x01,0x00,0x11,0x25,0x20,0x0C,0xAC,0xCD};

static struct etimer et;
typedef struct {
	uint8_t fcf[2];
	uint8_t seq_no;
	uint8_t pan_id[2];
	uint8_t dest_nwk_addr[2];
	uint8_t src_nwk_addr[2];
	uint8_t payload[MAX_PAYLOAD_SIZE];
} basicMACDataPkt_t;
uint8_t str[MAX_PAYLOAD_SIZE-20];
static basicMACDataPkt_t basicMACDataPkt;
static void set_rf_parameters(uint16_t pan_id, uint16_t nwk_addr, uint8_t *extd_addr, uint8_t channel);
static void buildMACHeader(uint16_t dest_addr);
static void prepare_payload(uint8_t *payload, uint8_t length);
static void basicMACDataPktSend(uint16_t dest_addr, uint8_t *payload, uint8_t payload_length);
/*---------------------------------------------------------------------------*/
PROCESS(cmote_tx_process, "C-Mote TX process");
AUTOSTART_PROCESSES(&cmote_tx_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cmote_tx_process, ev, data)
{
  PROCESS_BEGIN();
	set_rf_parameters(PAN_ID, SRC_NWK_ADDR, ieee_addr, CHANNEL);
	prepare_payload(str, sizeof(str));
	NETSTACK_RADIO.on();
	while(1){
		etimer_set(&et, 1*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		basicMACDataPktSend(DEST_NWK_ADDR, str, sizeof(str));
		printf("Packet Transmitted\n");
		leds_toggle(LEDS_ALL);
	}
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

static void set_rf_parameters(uint16_t pan_id, uint16_t nwk_addr, uint8_t *extd_addr, uint8_t channel){

	cc2520_set_channel(channel);
	cc2520_set_pan_addr(pan_id, nwk_addr,extd_addr);
}

static void buildMACHeader(uint16_t dest_nwk_addr){

	basicMACDataPkt.fcf[0]			= 0x41;
	basicMACDataPkt.fcf[1]			= 0x88;
	basicMACDataPkt.pan_id[0]		= PAN_ID & 0xFF;
	basicMACDataPkt.pan_id[1]		= (PAN_ID >> 8) & (0xFF);
	basicMACDataPkt.dest_nwk_addr[0]	= dest_nwk_addr & 0xFF;
	basicMACDataPkt.dest_nwk_addr[1]	= (dest_nwk_addr >> 8) & (0xFF);
	basicMACDataPkt.src_nwk_addr[0]		= SRC_NWK_ADDR & 0xFF;
	basicMACDataPkt.src_nwk_addr[1]		= (SRC_NWK_ADDR >> 8) & (0xFF);
}
static void prepare_payload(uint8_t *payload, uint8_t length){

	memset(payload, 'D', length);
}
static void basicMACDataPktSend(uint16_t dest_nwk_addr, uint8_t *payload, uint8_t payload_length){
	
	buildMACHeader(dest_nwk_addr);
	basicMACDataPkt.seq_no++;
	memcpy(basicMACDataPkt.payload, payload, payload_length);
	NETSTACK_RADIO.send((uint8_t *)(&(basicMACDataPkt)), BASIC_MAC_HEADER_SIZE + payload_length);
}
