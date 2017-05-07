/*
 * ProbePacket.h
 *
 *  Created on: 07/05/2017
 *      Author: root
 */

#ifndef PROBEPACKET_H_
#define PROBEPACKET_H_

#include "../RawPacket.h"

namespace Packet {

class ProbePacket: public RawPacket {
public:
	enum EProbeType{
		PROBE_REQUEST = 4,
		PROBE_RESPONSE = 5,
	};

private:
	std::string mTxMac;
	std::string mRxMac;
	std::string mDstMac;
	EProbeType mType;
public:
	ProbePacket(const char* pckt, int size);
	virtual ~ProbePacket();

	const std::string& getTxMacAddr() const { return mTxMac; }
	const std::string& getRxMacAddr() const { return mRxMac; }
	const std::string& getDstMacAddr() const { return mDstMac; }
	EProbeType getType() const { return mType; }
};

} /* namespace Packet */

#endif /* PROBEPACKET_H_ */
