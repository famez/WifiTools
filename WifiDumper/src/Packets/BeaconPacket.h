/*
 * BeaconPacket.h
 *
 *  Created on: 01/05/2017
 *      Author: root
 */

#ifndef BEACONPACKET_H_
#define BEACONPACKET_H_

#include "../RawPacket.h"
#include <string>

namespace Packet {

class BeaconPacket: public RawPacket {
private:
	std::string mSSIDName;
public:
	BeaconPacket(const char* pckt, int size);
	virtual ~BeaconPacket();

	const std::string& getSSID() const { return mSSIDName; }
};

} /* namespace Packet */

#endif /* BEACONPACKET_H_ */
