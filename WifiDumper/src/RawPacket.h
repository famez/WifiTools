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
