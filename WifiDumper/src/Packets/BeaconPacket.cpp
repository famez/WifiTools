/*
 * BeaconPacket.cpp
 *
 *  Created on: 01/05/2017
 *      Author: root
 */

#include "BeaconPacket.h"
#include <string.h>

#define SSID_LENGTH_OFFSET		0x3F
#define SSID_NAME_OFFSET		0x40

namespace Packet {

BeaconPacket::BeaconPacket(const char* pckt, int size) : RawPacket(pckt, size) {

	int ssidLength = mRaw[SSID_LENGTH_OFFSET];

	char* ssid = new char[ssidLength + 1];

	memcpy(ssid, mRaw + SSID_NAME_OFFSET, ssidLength);

	ssid[ssidLength] = 0;

	mSSIDName = ssid;

	delete[] ssid;

}

BeaconPacket::~BeaconPacket() {
}

} /* namespace Packet */
