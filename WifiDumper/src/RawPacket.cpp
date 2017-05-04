/*
 * RawPacket.cpp
 *
 *  Created on: 01/05/2017
 *      Author: root
 */

#include <string.h>
#include "RawPacket.h"

namespace Packet {


std::string macToStr(const char* mac)
{
	std::string retVal;
	if(mac)
	{
		char macStr[1024];
		memset(macStr, 0, 1024);
		sprintf(macStr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		retVal = macStr;
	}
	return retVal;
}

RawPacket::RawPacket(const char* pckt, int size) : mSize(size){
	mRaw = new char[size];
	::memcpy(mRaw, pckt, size);
}

RawPacket::~RawPacket() {
}

} /* namespace Packet */
