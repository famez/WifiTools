//============================================================================
// Name        : WifiDumper.cpp
// Author      : Famez
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "WifiManager.h"

#include <iostream>
#include <map>

#include "Packets/BeaconPacket.h"
#include "Packets/HandShakePacket.h"
#include "Packets/ProbePacket.h"


using namespace Wifi;
using namespace Packet;


void handleProbePacket(RawPacket& raw)
{
	ProbePacket packet(raw.getRawData(), raw.getSize());

	std::string type = "Unknown";

	switch(packet.getType())
	{
	case ProbePacket::PROBE_REQUEST:
		type = "Request";
		break;
	case ProbePacket::PROBE_RESPONSE:
		type = "Response";
		break;
	default:
		break;
	}

	std::cout << "Probe packet received. " << "Type: " << type << " From: " << packet.getRxMacAddr() << " To: " << packet.getTxMacAddr() << std::endl;

}

void handleBeaconPacket(RawPacket& raw)
{
	std::cout << "Processing beacon..." << std::endl;

	BeaconPacket packet(raw.getRawData(), raw.getSize());

	std::cout << "SSID is: " << packet.getSSID() << std::endl;

}


void handleHandShakePacket(RawPacket& raw)
{
	std::cout << "Handshake detected." << std::endl;
	HandShakePacket packet(raw.getRawData(), raw.getSize());

	std::cout << "TX addr: " << packet.getTxMacAddr() << " Rx Addr: " << packet.getRxMacAddr()
			<< " Dst addr: " << packet.getDstMacAddr() << std::endl;

}

int main() {

	std::map<WirelessMode, std::string> modeToStr;
	modeToStr[WIRELESS_MODE_MONITOR] = "Monitor mode";
	modeToStr[WIRELESS_MODE_STATION] = "Station mode";
	modeToStr[WIRELESS_MODE_AP] = "AP mode";
	modeToStr[WIRELESS_MODE_UNKNOWN] = "Unknown mode";


	std::set<std::string> wlans = WifiManager::getWirelessInterfaces();

	if(!wlans.empty())
	{
		std::cout << "Type one of the following interfaces to listen to: " << std::endl;
	}else{
		std::cout << "No wireless interfaces" << std::endl;
		return 0;
	}

	for(std::set<std::string>::const_iterator wlan = wlans.begin(); wlan != wlans.end(); ++wlan)
	{
		std::cout << *wlan;
	}
	std::cout << std::endl;

	std::string wlanIface = "wlan0";

	std::cin >> wlanIface;

	if(wlans.find(wlanIface) == wlans.end())
	{
		std::cerr << "Not valid interface" << std::endl;
		return 2;
	}

	std::cout << "Configuring interface to start dumping interesting traffic..." << std::endl;

	WifiManager wifiManager(wlanIface);

	WirelessMode mode = wifiManager.getMode();

	if(mode == WIRELESS_MODE_UNKNOWN)
		return 3;

	std::cout << "Mode: " << modeToStr[mode] << std::endl;

	if(mode != WIRELESS_MODE_MONITOR)
	{
		std::cout << "Necessary to change to monitor mode..." << std::endl;

		std::cout << "Checking if interface " << wifiManager.getIfName() << " is down..." << std::endl;

		if(wifiManager.isIfUp())
		{
			std::cout	<< "Interface is up. Down interface" << std::endl;
			if(wifiManager.downIf())
			{
				std::cout << "Now if is down" << std::endl;
			}
			else{
				std::cerr << "Error turning down interface..." << std::endl;
				return 4;
			}
		}

		std::cout << "Setting monitor mode to fetch traffic..." << std::endl;
		bool result = wifiManager.setMode(WIRELESS_MODE_MONITOR);
		if(!result)
		{
			std::cerr << "Error setting monitor mode..." << std::endl;
			return 5;
		}

	}

	if(!wifiManager.isIfUp())
	{
		std::cout << "Turning iface up" << std::endl;
		if(!wifiManager.upIf())
		{
			std::cerr << "Error turning up iface..." << std::endl;
			return 6;
		}
	}


	std::cout << "Everything ready to dump traffic..." << std::endl;

	std::cout << "What do you want to filter?" << std::endl;

	std::cout << "[1] Beacon" << std::endl;

	std::cout << "[2] Handshake" << std::endl;

	std::cout << "[3] Probe" << std::endl;

	int option = 0;

	std::cin >> option;

	switch(option)
	{
	case 1:
		wifiManager.registerHandler(FILTER_BEACON_FRAMES, &handleBeaconPacket);
		break;
	case 2:
		wifiManager.registerHandler(FILTER_HANDSHAKE_FRAMES, &handleHandShakePacket);
		break;
	case 3:
		wifiManager.registerHandler(FILTER_PROBE_FRAMES, &handleProbePacket);
		break;
	}

	std::cout << "Registering handlers" << std::endl;









	std::cout << "Starting to sniff" << std::endl;



	wifiManager.sniff();


	return 0;
}

