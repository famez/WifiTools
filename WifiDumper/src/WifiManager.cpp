/*
 * WifiManager.cpp
 *
 *  Created on: 30/04/2017
 *      Author: root
 */

#include "WifiManager.h"

#include <iostream>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/filter.h>
#include <linux/wireless.h>


#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>

#include <ifaddrs.h>
#include <string.h>

#include "Filters.h"



#define WIFI_802_11		"802.11"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

using namespace Packet;

namespace Wifi{


WifiManager::WifiManager(const std::string& iface) : mIfName(iface)
{
	s = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
}

WifiManager::~WifiManager() {
	close(s);
	for(std::set<PacketHelper>::const_iterator iter = mHelpers.begin(); iter != mHelpers.end(); ++iter)
	{
		iter->closeSocket();
	}
}

std::set<std::string>
WifiManager::getWirelessInterfaces()
{
	struct ifaddrs *addrs,*tmp;
	struct ifreq ifr;
	std::set<std::string> wlanIfaces;
	SOCKET s = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp)
	{
		strncpy(ifr.ifr_ifrn.ifrn_name, tmp->ifa_name, IFNAMSIZ);

		if(ioctl(s, SIOCGIWNAME, &ifr)==0)		//Wlan extension
		{
			struct iwreq* iwr = (struct iwreq*)&ifr;

			std::string proto = iwr->u.name;

			if(proto.find(WIFI_802_11) != std::string::npos)
			{
				wlanIfaces.insert(tmp->ifa_name);
			}


		}
	    tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
	close(s);

	return wlanIfaces;

}

bool
WifiManager::setMode(WirelessMode mode)
{
	struct iwreq iwr;
	nl80211_iftype type = (nl80211_iftype)mode;
	strncpy(iwr.ifr_ifrn.ifrn_name, mIfName.c_str(), IFNAMSIZ);

	iwr.u.mode = type;

	if(ioctl(s, SIOCSIWMODE, &iwr) != 0)
	{
		std::cerr << "Error setting mode. Error: " << strerror(errno) << std::endl;
		return false;
	}
	return true;

}


WirelessMode
WifiManager::getMode()
{
	struct iwreq iwr;
	strncpy(iwr.ifr_ifrn.ifrn_name, mIfName.c_str(), IFNAMSIZ);

	if(ioctl(s, SIOCGIWMODE, &iwr)!=0)
	{
		std::cerr << "Error getting mode";
		return WIRELESS_MODE_UNKNOWN;

	}

	nl80211_iftype type = (nl80211_iftype)iwr.u.mode;

	switch(type)
	{
	case NL80211_IFTYPE_MONITOR:
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_AP:
		break;
	default:
		std::cerr << "Mode not recognized";
		return WIRELESS_MODE_UNKNOWN;
	}

	WirelessMode mode = (WirelessMode)type;
	return mode;

}

bool
WifiManager::isIfUp()
{
	return __getIfFlags() & IFF_UP;
}

bool
WifiManager::__setIfFlag(short flag, bool value)
{
	struct ifreq ifr;
	strncpy(ifr.ifr_ifrn.ifrn_name, mIfName.c_str(), IFNAMSIZ);

	short flags = __getIfFlags();

	if(value)
	{
		flags |= flag;
	}else{
		flags &= ~(flag);
	}

	ifr.ifr_ifru.ifru_flags = flags;

	if(ioctl(s, SIOCSIFFLAGS, &ifr)!=0)
	{
		std::cerr << "Error setting flags for" << mIfName << std::endl;
		return false;
	}
	return true;
}

short
WifiManager::__getIfFlags()
{
	struct ifreq ifr;
	strncpy(ifr.ifr_ifrn.ifrn_name, mIfName.c_str(), IFNAMSIZ);

	if(ioctl(s, SIOCGIFFLAGS, &ifr)!=0)
	{
		std::cerr << "Error getting flags for" << mIfName << std::endl;
		return 0;
	}
	return ifr.ifr_ifru.ifru_flags;
}


void
WifiManager::sniff()
{
	char packet_data[1024];


	if(!mHelpers.empty())
	{
		while(true)
		{
			fd_set readfds;
			FD_ZERO( &readfds );
			SOCKET maxSck = -1;
			for(std::set<PacketHelper>::const_iterator iter = mHelpers.begin(); iter != mHelpers.end(); ++iter)
			{
				FD_SET(iter->getSocket(), &readfds );
				maxSck = iter->getSocket();		//Ordered by socket fd, so the last one will have the greatest sckt
			}

			int numFds = select(maxSck+1, &readfds, NULL, NULL, NULL );

			if(numFds<0)
			{
				std::cerr << "Error in select function. Error: " << strerror(errno) << std::endl;
			}


			for(std::set<PacketHelper>::const_iterator iter = mHelpers.begin(); iter != mHelpers.end(); ++iter)
			{
				if(FD_ISSET(iter->getSocket(), &readfds))
				{
					int caplen = read(iter->getSocket(), packet_data, sizeof(packet_data));
					if(caplen > 0 && iter->getCallback()){
						RawPacket packet(packet_data, caplen);
						handle_packet handler = iter->getCallback();
						handler(packet);
					}
					if(caplen<0)
					{
						std::cerr << "Error reading from socket. Error: " << strerror(errno) << std::endl;
					}
				}
			}
		}
	}
}

WifiManager::PacketHelper::PacketHelper(EFilterType type, const std::string& ifName) : mType(type), mFilterCode(NULL), mFilterSize(0), mSocket(-1), mCallBack(NULL)
{
	switch(type)
	{
	case FILTER_BEACON_FRAMES:
	{
		mFilterCode = Filters::beacon_filter_code;
		mFilterSize = sizeof(Filters::beacon_filter_code) / sizeof(struct sock_filter);
		break;
	}
	case FILTER_PROBE_FRAMES:
	{
		mFilterCode = Filters::probe_filter_code;
		mFilterSize = sizeof(Filters::probe_filter_code) / sizeof(struct sock_filter);
		break;
	}
	case FILTER_HANDSHAKE_FRAMES:
	{
		mFilterCode = Filters::handshake_filter_code;
		mFilterSize = sizeof(Filters::handshake_filter_code) / sizeof(struct sock_filter);
		break;
	}
	default:
		break;
	}

	struct ifreq ifr;
	struct sockaddr_ll ll;

	mSocket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	struct sock_fprog bpf = {
			(__u16)mFilterSize,
			mFilterCode
	};

	int res=setsockopt(mSocket, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
	if ( res < 0 )
	{
		std::cerr << "Error setsockopt failed" << std::endl;
		return;
	}

	memset( &ifr, 0, sizeof( ifr ) );
	strncpy( ifr.ifr_name, ifName.c_str(), sizeof(ifr.ifr_name));
	if (ioctl(mSocket, SIOCGIFINDEX, &ifr) < 0)
	{
		std::cerr << "Error ioctl failed" << std::endl;
		return;
	}

	memset( &ll, 0, sizeof(ll) );
	ll.sll_family = AF_PACKET;
	ll.sll_ifindex = ifr.ifr_ifindex;
	ll.sll_protocol = htons(ETH_P_ALL);
	if ( bind( mSocket, (struct sockaddr *) &ll, sizeof(ll) ) < 0 ) {
		std::cerr << "Error bind failed" << std::endl;
	}
}


WifiManager::PacketHelper::~PacketHelper()
{
}



void
WifiManager::registerHandler(EFilterType type, handle_packet handler)
{
	PacketHelper helper(type, mIfName);
	helper.setCallback(handler);
	mHelpers.insert(helper);
}


void
WifiManager::PacketHelper::closeSocket() const
{
	close(mSocket);
}

void
WifiManager::deauthenticate(const std::string& remoteAddr, const std::string& ssid)
{

}

}
