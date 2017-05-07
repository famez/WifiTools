/*
 * ProbePacket.cpp
 *
 *  Created on: 07/05/2017
 *      Author: root
 */

#include "ProbePacket.h"

#define PROBE_MASK	0xF
#define PROBE_SHIFT	4

namespace Packet {

ProbePacket::ProbePacket(const char* pckt, int size) : RawPacket(pckt, size){

	u16 radioTapHeaderLength = mRaw[RADIO_TAP_HEADER_LENGTH_OFFSET] | (mRaw[RADIO_TAP_HEADER_LENGTH_OFFSET + 1] << 8);

	char* wifiData = mRaw + radioTapHeaderLength;

	mType = (EProbeType)((wifiData[0] >> PROBE_SHIFT) & PROBE_MASK);

	mRxMac = macToStr(wifiData + RX_MAC_OFFSET);
	mTxMac = macToStr(wifiData + TX_MAC_OFFSET);
	mDstMac = macToStr(wifiData + DST_MAC_OFFSET);



}

ProbePacket::~ProbePacket() {
}

} /* namespace Packet */
