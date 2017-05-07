/*
 * RawPacket.h
 *
 *  Created on: 01/05/2017
 *      Author: root
 */

#ifndef RAWPACKET_H_
#define RAWPACKET_H_

#include "types.h"
#include <string>
#include <string.h>
#include <stdio.h>

#define MAC_ADDR_LENGTH						6
#define RADIO_TAP_HEADER_LENGTH_OFFSET		0x02

#define RX_MAC_OFFSET						0x04
#define TX_MAC_OFFSET						(RX_MAC_OFFSET + RADIO_TAP_HEADER_LENGTH_OFFSET)
#define DST_MAC_OFFSET						(TX_MAC_OFFSET + MAC_ADDR_LENGTH)

namespace Packet {

std::string macToStr(const char* mac);

class RawPacket {
protected:
	char* mRaw;
	int mSize;
public:
	RawPacket(const char* pckt, int size);
	virtual ~RawPacket();
	const char* getRawData() const { return mRaw; }
	int getSize() const { return mSize; }
};

} /* namespace Packet */

#endif /* RAWPACKET_H_ */
