/*
 * HandShakePacket.cpp
 *
 *  Created on: 02/05/2017
 *      Author: root
 */

#include "HandShakePacket.h"



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
