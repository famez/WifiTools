/*
 * HandShakePacket.h
 *
 *  Created on: 02/05/2017
 *      Author: root
 */

#ifndef HANDSHAKEPACKET_H_
#define HANDSHAKEPACKET_H_

#include <netinet/in.h>

#include "../RawPacket.h"
#include "../types.h"



#define NONCE_LENGTH		0x20
#define IV_LENGTH			0x10
#define KEY_RSC_LENGTH		8
#define KEY_ID_LENGTH		8
#define KEY_MIC_LENGTH		0x10

namespace Packet {


struct _802_1X_Auth_Info {
	 u8 version;
	 u8 type;
	 u16 length;

};

struct key_description {
	u8 descriptor_type;
	struct key_info {
		u16 notUsed : 3;
		u16 encrypted_key_data : 1;
		u16 request : 1;
		u16 error : 1;
		u16 secure : 1;
		u16 key_mic : 1;
		u16 key_ack : 1;
		u16 install : 1;
		u16 key_index : 1;
		u16 key_type : 1;
		u16 key_descriptor_version : 1;
	}key_info;
	u16 key_length;
	u32 replay_counterh;
	u32 replay_counterl;
	u8 nonce[NONCE_LENGTH];
	u8 iv[IV_LENGTH];
	u8 key_rsc[KEY_RSC_LENGTH];
	u8 key_id[KEY_ID_LENGTH];
	u8 key_mic[KEY_MIC_LENGTH];
	u16 key_data_length;
};


class HandShakePacket: public RawPacket {
private:
	_802_1X_Auth_Info mInfo;
	key_description mKeyDesc;
	std::string mTxMac;
	std::string mRxMac;
	std::string mDstMac;

public:
	HandShakePacket(const char* pckt, int size);
	virtual ~HandShakePacket();
	bool keyMICSet() { return mKeyDesc.key_info.key_mic; }
	bool keyACkSet() { return mKeyDesc.key_info.key_ack; }
	bool keyIsPairwise() { return mKeyDesc.key_info.key_type; }
	const std::string& getTxMacAddr() { return mTxMac; }
	const std::string& getRxMacAddr() { return mRxMac; }
	const std::string& getDstMacAddr() { return mDstMac; }

};

} /* namespace Packet */

#endif /* HANDSHAKEPACKET_H_ */
