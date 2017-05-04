/*
 * HandShakePacket.cpp
 *
 *  Created on: 02/05/2017
 *      Author: root
 */

#include "HandShakePacket.h"

#define MAC_ADDR_LENGTH						6
#define RADIO_TAP_HEADER_LENGTH_OFFSET		0x02

#define RX_MAC_OFFSET						0x04
#define TX_MAC_OFFSET						(RX_MAC_OFFSET + RADIO_TAP_HEADER_LENGTH_OFFSET)
#define DST_MAC_OFFSET						(TX_MAC_OFFSET + MAC_ADDR_LENGTH)


namespace Packet {

HandShakePacket::HandShakePacket(const char* pckt, int size) : RawPacket(pckt, size) {

	u16 radioTapHeaderLength = mRaw[RADIO_TAP_HEADER_LENGTH_OFFSET] | (mRaw[RADIO_TAP_HEADER_LENGTH_OFFSET + 1] << 8);

	char* wifiData = mRaw + radioTapHeaderLength;

	mInfo = *((_802_1X_Auth_Info*)(wifiData + 0x22));
	mKeyDesc = *((key_description*)(wifiData + 0x22 + sizeof(_802_1X_Auth_Info)));

	mRxMac = macToStr(wifiData + RX_MAC_OFFSET);
	mTxMac = macToStr(wifiData + TX_MAC_OFFSET);
	mDstMac = macToStr(wifiData + DST_MAC_OFFSET);
}

HandShakePacket::~HandShakePacket() {
}

} /* namespace Packet */
