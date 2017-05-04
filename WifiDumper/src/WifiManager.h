/*
 * WifiManager.h
 *
 *  Created on: 30/04/2017
 *      Author: root
 */

#ifndef WIFIMANAGER_H_
#define WIFIMANAGER_H_

#include <stdio.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/nl80211.h>


#include <string>
#include <set>

#include "types.h"
#include "RawPacket.h"

typedef int SOCKET;

struct sock_filter;

namespace Wifi {

typedef void (*handle_packet)(Packet::RawPacket&);

enum WirelessMode
{
	WIRELESS_MODE_MONITOR = 	NL80211_IFTYPE_MONITOR,
	WIRELESS_MODE_STATION = 	NL80211_IFTYPE_STATION,
	WIRELESS_MODE_AP = 			NL80211_IFTYPE_AP,
	WIRELESS_MODE_UNKNOWN = 	NL80211_IFTYPE_UNSPECIFIED,

};

enum EFilterType{
	FILTER_BEACON_FRAMES,
	FILTER_PROBE_FRAMES,
	FILTER_HANDSHAKE_FRAMES,
	FILTER_MAX_FILTERS
};

class WifiManager {
private:
	std::string mIfName;
	SOCKET s;

	bool __setIfFlag(short flag, bool value);
	short __getIfFlags();

	class PacketHelper{
	private:
		EFilterType mType;
		struct sock_filter* mFilterCode;
		int mFilterSize;
		SOCKET mSocket;
		handle_packet mCallBack;
	public:
		PacketHelper(EFilterType flag, const std::string& ifName);
		virtual ~PacketHelper();
		SOCKET getSocket() const { return mSocket; }
		void closeSocket() const;
		EFilterType getType() const { return mType; }
		void setCallback(handle_packet callback) { mCallBack = callback; }
		handle_packet getCallback() const { return mCallBack; }
		bool operator <(const PacketHelper& other) const { return mSocket < other.mSocket; }
	};
	std::set<PacketHelper> mHelpers;

public:
	WifiManager(const std::string& iface);
	virtual ~WifiManager();

	static std::set<std::string> getWirelessInterfaces();

	const std::string& getIfName() { return mIfName; }

	bool setMode(WirelessMode);
	WirelessMode getMode();

	bool isIfUp();

	bool upIf() {return __setIfFlag(IFF_UP, true);}
	bool downIf() {return __setIfFlag(IFF_UP, false);}

	void sniff();

	void registerHandler(EFilterType type, handle_packet handler);

};

}		//End namespace Wifi

#endif /* WIFIMANAGER_H_ */
