﻿// This code is part of Pcap_DNSProxy
// A local DNS server based on WinPcap and LibPcap
// Copyright (C) 2012-2015 Chengr28
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "Configuration.h"

//Check parameter list and set default values
bool __fastcall ParameterCheckAndSetting(const size_t FileIndex)
{
//Version check
	if (Parameter.Version > CONFIG_VERSION)
	{
		PrintError(LOG_ERROR_PARAMETER, L"Configuration file version error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
	else if (Parameter.Version < CONFIG_VERSION)
	{
		PrintError(LOG_MESSAGE_NOTICE, L"Configuration file is not the latest version", 0, nullptr, 0);
	}

//Log max size check
	if (Parameter.LogMaxSize < DEFAULT_LOG_MINSIZE || Parameter.LogMaxSize > DEFAULT_FILE_MAXSIZE)
	{
		PrintError(LOG_ERROR_PARAMETER, L"Log file size error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}

//DNS Main and Alternate targets check
	if (Parameter.ListenAddress_IPv6->empty())
	{
		delete Parameter.ListenAddress_IPv6;
		Parameter.ListenAddress_IPv6 = nullptr;
	}
	if (Parameter.ListenAddress_IPv4->empty())
	{
		delete Parameter.ListenAddress_IPv4;
		Parameter.ListenAddress_IPv4 = nullptr;
	}
	if (!Parameter.EDNS_ClientSubnet)
	{
		delete Parameter.LocalhostSubnet.IPv6;
		delete Parameter.LocalhostSubnet.IPv4;
		Parameter.LocalhostSubnet.IPv6 = nullptr;
		Parameter.LocalhostSubnet.IPv4 = nullptr;
	}
	if (!Parameter.DNSTarget.IPv6_Multi->empty())
	{
		Parameter.AlternateMultiRequest = true;

	//Copy DNS Server Data when Main or Alternate data are empty.
//	#if defined(ENABLE_PCAP)
		if (Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family == 0)
		{
			uint8_t HopLimitTemp = 0;
			if (Parameter.DNSTarget.IPv6.HopLimitData.HopLimit > 0)
				HopLimitTemp = Parameter.DNSTarget.IPv6.HopLimitData.HopLimit;
			Parameter.DNSTarget.IPv6 = Parameter.DNSTarget.IPv6_Multi->front();
			Parameter.DNSTarget.IPv6.HopLimitData.HopLimit = HopLimitTemp;
			Parameter.DNSTarget.IPv6_Multi->erase(Parameter.DNSTarget.IPv6_Multi->begin());
		}

		if (Parameter.DNSTarget.Alternate_IPv6.AddressData.Storage.ss_family == 0 && !Parameter.DNSTarget.IPv6_Multi->empty())
		{
			uint8_t HopLimitTemp = 0;
			if (Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit > 0)
				HopLimitTemp = Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit;
			Parameter.DNSTarget.Alternate_IPv6 = Parameter.DNSTarget.IPv6_Multi->front();
			Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit = HopLimitTemp;
			Parameter.DNSTarget.IPv6_Multi->erase(Parameter.DNSTarget.IPv6_Multi->begin());
		}
//	#endif

	//Multi select mode check
		if (Parameter.DNSTarget.IPv6_Multi->size() + 2U > FD_SETSIZE || //UDP requesting
			Parameter.RequestMode_Transport == REQUEST_MODE_TCP && (Parameter.DNSTarget.IPv6_Multi->size() + 2U) * Parameter.MultiRequestTimes > FD_SETSIZE) //TCP requesting
		{
			PrintError(LOG_ERROR_PARAMETER, L"Too many multi addresses", 0, ConfigFileList.at(FileIndex).c_str(), 0);
			return false;
		}

	//Multi DNS Server check
		if (Parameter.DNSTarget.IPv6_Multi->empty())
		{
			delete Parameter.DNSTarget.IPv6_Multi;
			Parameter.DNSTarget.IPv6_Multi = nullptr;
		}
	}
	else {
		delete Parameter.DNSTarget.IPv6_Multi;
		Parameter.DNSTarget.IPv6_Multi = nullptr;
	}
	if (!Parameter.DNSTarget.IPv4_Multi->empty())
	{
		Parameter.AlternateMultiRequest = true;

	//Copy DNS Server Data when Main or Alternate data are empty.
//	#if defined(ENABLE_PCAP)
		if (Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family == 0)
		{
			uint8_t TTLTemp = 0;
			if (Parameter.DNSTarget.IPv4.HopLimitData.TTL > 0)
				TTLTemp = Parameter.DNSTarget.IPv4.HopLimitData.TTL;
			Parameter.DNSTarget.IPv4 = Parameter.DNSTarget.IPv4_Multi->front();
			Parameter.DNSTarget.IPv4.HopLimitData.TTL = TTLTemp;
			Parameter.DNSTarget.IPv4_Multi->erase(Parameter.DNSTarget.IPv4_Multi->begin());
		}

		if (Parameter.DNSTarget.Alternate_IPv4.AddressData.Storage.ss_family == 0 && !Parameter.DNSTarget.IPv4_Multi->empty())
		{
			uint8_t TTLTemp = 0;
			if (Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL > 0)
				TTLTemp = Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL;
			Parameter.DNSTarget.Alternate_IPv4 = Parameter.DNSTarget.IPv4_Multi->front();
			Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL = TTLTemp;
			Parameter.DNSTarget.IPv4_Multi->erase(Parameter.DNSTarget.IPv4_Multi->begin());
		}
//	#endif

	//Multi select mode check
		if (Parameter.DNSTarget.IPv4_Multi->size() + 2U > FD_SETSIZE || //UDP requesting
			Parameter.RequestMode_Transport == REQUEST_MODE_TCP && (Parameter.DNSTarget.IPv4_Multi->size() + 2U) * Parameter.MultiRequestTimes > FD_SETSIZE) //TCP requesting
		{
			PrintError(LOG_ERROR_PARAMETER, L"Too many multi addresses", 0, ConfigFileList.at(FileIndex).c_str(), 0);
			return false;
		}

	//Multi DNS Server check
		if (Parameter.DNSTarget.IPv4_Multi->empty())
		{
			delete Parameter.DNSTarget.IPv4_Multi;
			Parameter.DNSTarget.IPv4_Multi = nullptr;
		}
	}
	else {
		delete Parameter.DNSTarget.IPv4_Multi;
		Parameter.DNSTarget.IPv4_Multi = nullptr;
	}
	if (Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Alternate_IPv6.AddressData.Storage.ss_family > 0)
	{
		Parameter.DNSTarget.IPv6 = Parameter.DNSTarget.Alternate_IPv6;
		memset(&Parameter.DNSTarget.Alternate_IPv6, 0, sizeof(DNS_SERVER_DATA));
	}
	if (Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Alternate_IPv4.AddressData.Storage.ss_family > 0)
	{
		Parameter.DNSTarget.IPv4 = Parameter.DNSTarget.Alternate_IPv4;
		memset(&Parameter.DNSTarget.Alternate_IPv4, 0, sizeof(DNS_SERVER_DATA));
	}
	if (Parameter.DNSTarget.Local_IPv6.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Alternate_Local_IPv6.AddressData.Storage.ss_family > 0)
	{
		Parameter.DNSTarget.Local_IPv6 = Parameter.DNSTarget.Alternate_Local_IPv6;
		memset(&Parameter.DNSTarget.Alternate_Local_IPv6, 0, sizeof(DNS_SERVER_DATA));
	}
	if (Parameter.DNSTarget.Local_IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Alternate_Local_IPv4.AddressData.Storage.ss_family > 0)
	{
		Parameter.DNSTarget.Local_IPv4 = Parameter.DNSTarget.Alternate_Local_IPv4;
		memset(&Parameter.DNSTarget.Alternate_Local_IPv4, 0, sizeof(DNS_SERVER_DATA));
	}
	if (Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family == 0 || 
	//Check repeating items.
		Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family > 0 && Parameter.DNSTarget.Alternate_IPv4.AddressData.Storage.ss_family > 0 && 
		Parameter.DNSTarget.IPv4.AddressData.IPv4.sin_addr.s_addr == Parameter.DNSTarget.Alternate_IPv4.AddressData.IPv4.sin_addr.s_addr && Parameter.DNSTarget.IPv4.AddressData.IPv4.sin_port == Parameter.DNSTarget.Alternate_IPv4.AddressData.IPv4.sin_port || 
		Parameter.DNSTarget.Local_IPv4.AddressData.Storage.ss_family > 0 && Parameter.DNSTarget.Alternate_Local_IPv4.AddressData.Storage.ss_family > 0 && 
		Parameter.DNSTarget.Local_IPv4.AddressData.IPv4.sin_addr.s_addr == Parameter.DNSTarget.Alternate_Local_IPv4.AddressData.IPv4.sin_addr.s_addr && Parameter.DNSTarget.Local_IPv4.AddressData.IPv4.sin_port == Parameter.DNSTarget.Alternate_Local_IPv4.AddressData.IPv4.sin_port || 
		Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family > 0 && Parameter.DNSTarget.Alternate_IPv6.AddressData.Storage.ss_family > 0 && 
		memcmp(&Parameter.DNSTarget.IPv6.AddressData.IPv6.sin6_addr, &Parameter.DNSTarget.Alternate_IPv6.AddressData.IPv6.sin6_addr, sizeof(in6_addr)) == EXIT_SUCCESS && Parameter.DNSTarget.IPv6.AddressData.IPv6.sin6_port == Parameter.DNSTarget.Alternate_IPv6.AddressData.IPv6.sin6_port || 
		Parameter.DNSTarget.Local_IPv6.AddressData.Storage.ss_family > 0 && Parameter.DNSTarget.Alternate_Local_IPv6.AddressData.Storage.ss_family > 0 && 
		memcmp(&Parameter.DNSTarget.Local_IPv6.AddressData.IPv6.sin6_addr, &Parameter.DNSTarget.Alternate_Local_IPv6.AddressData.IPv6.sin6_addr, sizeof(in6_addr)) == EXIT_SUCCESS && Parameter.DNSTarget.Local_IPv6.AddressData.IPv6.sin6_port == Parameter.DNSTarget.Alternate_Local_IPv6.AddressData.IPv6.sin6_port)
	{
		PrintError(LOG_ERROR_PARAMETER, L"DNS target error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}

//Hop Limit or TTL Fluctuations check
#if defined(ENABLE_PCAP)
	if (Parameter.HopLimitFluctuation > 0)
	{
		//IPv6
		if (Parameter.DNSTarget.IPv6.HopLimitData.HopLimit > 0 && 
			((size_t)Parameter.DNSTarget.IPv6.HopLimitData.HopLimit + (size_t)Parameter.HopLimitFluctuation > UINT8_MAX || 
			(SSIZE_T)Parameter.DNSTarget.IPv6.HopLimitData.HopLimit < (SSIZE_T)Parameter.HopLimitFluctuation + 1) || 
			Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit > 0 && 
			((size_t)Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit + (size_t)Parameter.HopLimitFluctuation > UINT8_MAX || 
			(SSIZE_T)Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit < (SSIZE_T)Parameter.HopLimitFluctuation + 1) || 
		//IPv4
			Parameter.DNSTarget.IPv4.HopLimitData.TTL > 0 && 
			((size_t)Parameter.DNSTarget.IPv4.HopLimitData.TTL + (size_t)Parameter.HopLimitFluctuation > UINT8_MAX || 
			(SSIZE_T)Parameter.DNSTarget.IPv4.HopLimitData.TTL < (SSIZE_T)Parameter.HopLimitFluctuation + 1) || 
			Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL > 0 && 
			((size_t)Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL + (size_t)Parameter.HopLimitFluctuation > UINT8_MAX || 
			(SSIZE_T)Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL < (SSIZE_T)Parameter.HopLimitFluctuation + 1))
		{
			PrintError(LOG_ERROR_PARAMETER, L"Hop Limit Fluctuations error", 0, ConfigFileList.at(FileIndex).c_str(), 0); //Hop Limit and TTL must between 1 and 255.
			return false;
		}
	}
#endif

//Other errors which need to print to log.
#if defined(ENABLE_PCAP)
	#if defined(ENABLE_LIBSODIUM)
		if (!Parameter.PcapCapture && Parameter.DirectRequest != DIRECT_REQUEST_MODE_BOTH && !Parameter.DNSCurve && Parameter.RequestMode_Transport != REQUEST_MODE_TCP)
	#else
		if (!Parameter.PcapCapture && Parameter.DirectRequest != DIRECT_REQUEST_MODE_BOTH && Parameter.RequestMode_Transport != REQUEST_MODE_TCP)
	#endif
	{
		PrintError(LOG_ERROR_PARAMETER, L"Pcap Capture error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
#else
	#if defined(ENABLE_LIBSODIUM)
		if (Parameter.DirectRequest != DIRECT_REQUEST_MODE_BOTH && !Parameter.DNSCurve && Parameter.RequestMode_Transport != REQUEST_MODE_TCP)
	#else
		if (Parameter.DirectRequest != DIRECT_REQUEST_MODE_BOTH && Parameter.RequestMode_Transport != REQUEST_MODE_TCP)
	#endif
	{
		PrintError(LOG_ERROR_PARAMETER, L"Pcap Capture error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
#endif
	if ((Parameter.LocalMain || Parameter.LocalHosts || Parameter.LocalRouting) && 
		Parameter.DNSTarget.Local_IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Local_IPv6.AddressData.Storage.ss_family || 
		Parameter.LocalHosts && (Parameter.LocalMain || Parameter.LocalRouting) || Parameter.LocalRouting && !Parameter.LocalMain)
	{
		PrintError(LOG_ERROR_PARAMETER, L"Local Main / Local Hosts / Local Routing error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
	if (Parameter.CacheType > 0 && Parameter.CacheParameter == 0)
	{
		PrintError(LOG_ERROR_PARAMETER, L"DNS Cache error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
	if (Parameter.EDNSPayloadSize < DNS_PACKET_MAXSIZE_TRADITIONAL)
	{
		if (Parameter.EDNSPayloadSize > 0)
			PrintError(LOG_MESSAGE_NOTICE, L"EDNS Payload Size must longer than 512 bytes(Traditional DNS packet minimum supported size)", 0, nullptr, 0);
		Parameter.EDNSPayloadSize = EDNS_PACKET_MINSIZE;
	}
	else if (Parameter.EDNSPayloadSize >= PACKET_MAXSIZE - sizeof(ipv6_hdr) - sizeof(udp_hdr))
	{
		PrintError(LOG_MESSAGE_NOTICE, L"EDNS Payload Size may be too long", 0, nullptr, 0);
		Parameter.EDNSPayloadSize = EDNS_PACKET_MINSIZE;
	}
	if (Parameter.DNSTarget.Alternate_IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Alternate_IPv6.AddressData.Storage.ss_family == 0 && 
		Parameter.DNSTarget.Alternate_Local_IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Alternate_Local_IPv6.AddressData.Storage.ss_family == 0
	#if defined(ENABLE_LIBSODIUM)
		&& Parameter.DNSCurve && DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.Storage.ss_family == 0 && DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.Storage.ss_family == 0
	#endif
		)
	{
		PrintError(LOG_ERROR_PARAMETER, L"Alternate Multi requesting error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
	if (Parameter.MultiRequestTimes > MULTI_REQUEST_TIMES_MAXNUM)
	{
		PrintError(LOG_ERROR_PARAMETER, L"Multi requesting times error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		return false;
	}
	else if (Parameter.MultiRequestTimes < 1U)
	{
		Parameter.MultiRequestTimes = 1U;
	}

//Set values before check
#if defined(ENABLE_PCAP)
	if (Parameter.HeaderCheck_TCP) //TCP Mode option check
	{
		if (!Parameter.PcapCapture)
			PrintError(LOG_MESSAGE_NOTICE, L"TCP Data Filter require Pcap Cpature", 0, nullptr, 0);

		Parameter.HeaderCheck_TCP = false;
	}
	if (Parameter.HeaderCheck_IPv4) //IPv4 Data Filter option check
	{
		if (Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family == 0)
			PrintError(LOG_MESSAGE_NOTICE, L"IPv4 Data Filter require IPv4 DNS server", 0, nullptr, 0);
		if (!Parameter.PcapCapture)
			PrintError(LOG_MESSAGE_NOTICE, L"IPv4 Data Filter require Pcap Cpature", 0, nullptr, 0);

		Parameter.HeaderCheck_IPv4 = false;
	}
#endif

//DNSCurve options check
#if defined(ENABLE_LIBSODIUM)
	if (Parameter.DNSCurve)
	{
	//Libsodium initialization
		if (sodium_init() != EXIT_SUCCESS)
		{
			PrintError(LOG_ERROR_DNSCURVE, L"Libsodium initialization error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
			return false;
		}

	//Client keys check
		if (DNSCurveParameter.IsEncryption)
		{
			if (!CheckEmptyBuffer(DNSCurveParameter.Client_PublicKey, crypto_box_PUBLICKEYBYTES) && 
				!CheckEmptyBuffer(DNSCurveParameter.Client_SecretKey, crypto_box_SECRETKEYBYTES))
			{
				if (!DNSCurveVerifyKeypair(DNSCurveParameter.Client_PublicKey, DNSCurveParameter.Client_SecretKey))
				{
					PrintError(LOG_ERROR_DNSCURVE, L"Client keypair(public key and secret key) error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}
			}
			else {
				memset(DNSCurveParameter.Client_PublicKey, 0, crypto_box_PUBLICKEYBYTES);
				memset(DNSCurveParameter.Client_SecretKey, 0, crypto_box_SECRETKEYBYTES);
				crypto_box_curve25519xsalsa20poly1305_keypair(DNSCurveParameter.Client_PublicKey, DNSCurveParameter.Client_SecretKey);
			}
		}
		else {
			delete[] DNSCurveParameter.Client_PublicKey;
			delete[] DNSCurveParameter.Client_SecretKey;
			DNSCurveParameter.Client_PublicKey = nullptr;
			DNSCurveParameter.Client_SecretKey = nullptr;
		}

	//DNSCurve targets check
		if (DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage.ss_family == 0 && DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.Storage.ss_family > 0)
		{
			DNSCurveParameter.DNSCurveTarget.IPv6 = DNSCurveParameter.DNSCurveTarget.Alternate_IPv6;
			memset(&DNSCurveParameter.DNSCurveTarget.Alternate_IPv6, 0, sizeof(DNSCURVE_SERVER_DATA));
		}
		if (DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage.ss_family == 0 && DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.Storage.ss_family > 0)
		{
			DNSCurveParameter.DNSCurveTarget.IPv4 = DNSCurveParameter.DNSCurveTarget.Alternate_IPv4;
			memset(&DNSCurveParameter.DNSCurveTarget.Alternate_IPv4, 0, sizeof(DNSCURVE_SERVER_DATA));
		}

		if (DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage.ss_family == 0 && DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage.ss_family == 0 || 
		//Check repeating items.
			DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage.ss_family > 0 && DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.Storage.ss_family > 0 && 
			DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.IPv4.sin_addr.s_addr == DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.IPv4.sin_addr.s_addr || 
			DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage.ss_family > 0 && DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.Storage.ss_family > 0 && 
			memcmp(&DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.IPv6.sin6_addr, &DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.IPv6.sin6_addr, sizeof(in6_addr)) == EXIT_SUCCESS)
		{
			PrintError(LOG_ERROR_PARAMETER, L"DNSCurve target error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
			return false;
		}

	//Eencryption options check
		if (DNSCurveParameter.IsEncryptionOnly && !DNSCurveParameter.IsEncryption)
		{
			DNSCurveParameter.IsEncryption = true;
			PrintError(LOG_ERROR_PARAMETER, L"DNSCurve encryption options error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
		}

	//Main(IPv6)
		if (DNSCurveParameter.IsEncryption && DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage.ss_family > 0)
		{
		//Empty Server Fingerprint
			if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv6.ServerFingerprint, crypto_box_PUBLICKEYBYTES))
			{
			//Encryption Only mode check
				if (DNSCurveParameter.IsEncryptionOnly && 
					CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv6.SendMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve Encryption Only mode error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Provider Name
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv6.ProviderName, DOMAIN_MAXSIZE))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Provider Name error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Public Key
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv6.ServerPublicKey, crypto_box_PUBLICKEYBYTES))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Public Key error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}
			}
			else {
				crypto_box_curve25519xsalsa20poly1305_beforenm(
					DNSCurveParameter.DNSCurveTarget.IPv6.PrecomputationKey,
					DNSCurveParameter.DNSCurveTarget.IPv6.ServerFingerprint,
					DNSCurveParameter.Client_SecretKey);
			}
		}
		else {
			delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ProviderName;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv6.PrecomputationKey;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ServerPublicKey;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv6.SendMagicNumber;

			DNSCurveParameter.DNSCurveTarget.IPv6.ProviderName = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv6.PrecomputationKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv6.ServerPublicKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv6.SendMagicNumber = nullptr;
		}

	//Main(IPv4)
		if (DNSCurveParameter.IsEncryption && DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage.ss_family > 0)
		{
		//Empty Server Fingerprint
			if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv4.ServerFingerprint, crypto_box_PUBLICKEYBYTES))
			{
			//Encryption Only mode check
				if (DNSCurveParameter.IsEncryptionOnly && 
					CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv4.SendMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve Encryption Only mode error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Provider Name
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv4.ProviderName, DOMAIN_MAXSIZE))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Provider Name error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Public Key
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv4.ServerPublicKey, crypto_box_PUBLICKEYBYTES))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Public Key error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}
			}
			else {
				crypto_box_curve25519xsalsa20poly1305_beforenm(
					DNSCurveParameter.DNSCurveTarget.IPv4.PrecomputationKey,
					DNSCurveParameter.DNSCurveTarget.IPv4.ServerFingerprint,
					DNSCurveParameter.Client_SecretKey);
			}
		}
		else {
			delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ProviderName;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv4.PrecomputationKey;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ServerPublicKey;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber;
			delete[] DNSCurveParameter.DNSCurveTarget.IPv4.SendMagicNumber;

			DNSCurveParameter.DNSCurveTarget.IPv4.ProviderName = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv4.PrecomputationKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv4.ServerPublicKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber = nullptr;
			DNSCurveParameter.DNSCurveTarget.IPv4.SendMagicNumber = nullptr;
		}

	//Alternate(IPv6)
		if (DNSCurveParameter.IsEncryption && DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.Storage.ss_family > 0)
		{
		//Empty Server Fingerprint
			if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerFingerprint, crypto_box_PUBLICKEYBYTES))
			{
			//Encryption Only mode check
				if (DNSCurveParameter.IsEncryptionOnly && 
					CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.SendMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve Encryption Only mode error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Provider Name
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ProviderName, DOMAIN_MAXSIZE))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Provider Name error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Public Key
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerPublicKey, crypto_box_PUBLICKEYBYTES))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Public Key error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}
			}
			else {
				crypto_box_curve25519xsalsa20poly1305_beforenm(
					DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.PrecomputationKey,
					DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerFingerprint,
					DNSCurveParameter.Client_SecretKey);
			}
		}
		else {
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ProviderName;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.PrecomputationKey;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerPublicKey;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.SendMagicNumber;

			DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ProviderName = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.PrecomputationKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerPublicKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.SendMagicNumber = nullptr;
		}

	//Alternate(IPv4)
		if (DNSCurveParameter.IsEncryption && DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.Storage.ss_family > 0)
		{
		//Empty Server Fingerprint
			if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerFingerprint, crypto_box_PUBLICKEYBYTES))
			{
			//Encryption Only mode check
				if (DNSCurveParameter.IsEncryptionOnly && 
					CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.SendMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve Encryption Only mode error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Provider Name
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ProviderName, DOMAIN_MAXSIZE))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Provider Name error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}

			//Empty Public Key
				if (CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerPublicKey, crypto_box_PUBLICKEYBYTES))
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve empty Public Key error", 0, ConfigFileList.at(FileIndex).c_str(), 0);
					return false;
				}
			}
			else {
				crypto_box_curve25519xsalsa20poly1305_beforenm(
					DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.PrecomputationKey,
					DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerFingerprint,
					DNSCurveParameter.Client_SecretKey);
			}
		}
		else {
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ProviderName;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.PrecomputationKey;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerPublicKey;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber;
			delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.SendMagicNumber;

			DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ProviderName = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.PrecomputationKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerPublicKey = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber = nullptr;
			DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.SendMagicNumber = nullptr;
		}
	}
	else {
		delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ProviderName;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ProviderName;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ProviderName;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ProviderName;
	//DNSCurve Keys
		delete[] DNSCurveParameter.Client_PublicKey;
		delete[] DNSCurveParameter.Client_SecretKey;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv4.PrecomputationKey;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.PrecomputationKey;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv6.PrecomputationKey;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.PrecomputationKey;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ServerPublicKey;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerPublicKey;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ServerPublicKey;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerPublicKey;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ServerFingerprint;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerFingerprint;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ServerFingerprint;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerFingerprint;
	//DNSCurve Magic Numbers
		delete[] DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv4.SendMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.SendMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.IPv6.SendMagicNumber;
		delete[] DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.SendMagicNumber;

		DNSCurveParameter.DNSCurveTarget.IPv4.ProviderName = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ProviderName = nullptr, DNSCurveParameter.DNSCurveTarget.IPv6.ProviderName = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ProviderName = nullptr;
		DNSCurveParameter.Client_PublicKey = nullptr, DNSCurveParameter.Client_SecretKey = nullptr;
		DNSCurveParameter.DNSCurveTarget.IPv4.PrecomputationKey = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.PrecomputationKey = nullptr, DNSCurveParameter.DNSCurveTarget.IPv6.PrecomputationKey = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.PrecomputationKey = nullptr;
		DNSCurveParameter.DNSCurveTarget.IPv4.ServerPublicKey = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerPublicKey = nullptr, DNSCurveParameter.DNSCurveTarget.IPv6.ServerPublicKey = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerPublicKey = nullptr;
		DNSCurveParameter.DNSCurveTarget.IPv4.ServerFingerprint = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerFingerprint = nullptr, DNSCurveParameter.DNSCurveTarget.IPv6.ServerFingerprint = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerFingerprint = nullptr;
		DNSCurveParameter.DNSCurveTarget.IPv4.SendMagicNumber = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.SendMagicNumber = nullptr, DNSCurveParameter.DNSCurveTarget.IPv6.SendMagicNumber = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.SendMagicNumber = nullptr;
		DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber = nullptr, DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber = nullptr, DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber = nullptr;
	}
#endif

//Default settings
	//Listen Port
	if (Parameter.ListenPort != nullptr && Parameter.ListenPort->empty())
	{
		PrintError(LOG_MESSAGE_NOTICE, L"Listen Port is empty, set to standard DNS port(53)", 0, nullptr, 0);
		Parameter.ListenPort->push_back(htons(IPPORT_DNS));
	}

	//Protocol
	if (Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Local_IPv6.AddressData.Storage.ss_family == 0
	#if defined(ENABLE_LIBSODIUM)
		&& DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage.ss_family == 0
	#endif
		)
	{
		PrintError(LOG_MESSAGE_NOTICE, L"IPv6 Request Mode require IPv6 DNS server", 0, nullptr, 0);
		Parameter.RequestMode_Network = REQUEST_MODE_NETWORK_BOTH;
	}
	if (Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family == 0 && Parameter.DNSTarget.Local_IPv4.AddressData.Storage.ss_family == 0
	#if defined(ENABLE_LIBSODIUM)
		&& DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage.ss_family == 0
	#endif
		)
	{
		PrintError(LOG_MESSAGE_NOTICE, L"IPv4 Request Mode require IPv4 DNS server", 0, nullptr, 0);
		Parameter.RequestMode_Network = REQUEST_MODE_NETWORK_BOTH;
	}

	//EDNS Label
	if (Parameter.DNSSEC_ForceValidation && (!Parameter.EDNS_Label || !Parameter.DNSSEC_Request || !Parameter.DNSSEC_Validation))
	{
		PrintError(LOG_MESSAGE_NOTICE, L"DNSSEC Force Validation require EDNS Label, DNSSEC Request and DNSSEC Validation", 0, nullptr, 0);
		Parameter.EDNS_Label = true;
		Parameter.DNSSEC_Request = true;
		Parameter.DNSSEC_Validation = true;
	}
	if (Parameter.DNSSEC_Validation && (!Parameter.EDNS_Label || !Parameter.DNSSEC_Request))
	{
		PrintError(LOG_MESSAGE_NOTICE, L"DNSSEC Validation require EDNS Label and DNSSEC Request", 0, nullptr, 0);
		Parameter.EDNS_Label = true;
		Parameter.DNSSEC_Request = true;
	}
	if (!Parameter.EDNS_Label)
	{
		if (Parameter.EDNS_ClientSubnet)
		{
			PrintError(LOG_MESSAGE_NOTICE, L"EDNS Client Subnet require EDNS Label", 0, nullptr, 0);
			Parameter.EDNS_Label = true;
		}
		if (Parameter.DNSSEC_Request)
		{
			PrintError(LOG_MESSAGE_NOTICE, L"DNSSEC Request require EDNS Label", 0, nullptr, 0);
			Parameter.EDNS_Label = true;
		}
	}
	else {
		if (Parameter.CompressionPointerMutation)
		{
			PrintError(LOG_MESSAGE_NOTICE, L"Turn OFF Compression Pointer Mutation when EDNS Label is available", 0, nullptr, 0);
			Parameter.CompressionPointerMutation = false;
		}
	}

	//Domain Test
#if defined(ENABLE_PCAP)
	if (CheckEmptyBuffer(Parameter.DomainTest_Data, DOMAIN_MAXSIZE))
	{
		delete[] Parameter.DomainTest_Data;
		Parameter.DomainTest_Data = nullptr;
	}
#endif

	//Default Local DNS server name
	if (Parameter.LocalFQDN_Length <= 0)
	{
		Parameter.LocalFQDN_Length = CharToDNSQuery(DEFAULT_LOCAL_SERVERNAME, Parameter.LocalFQDN_Response);
		*Parameter.LocalFQDN_String = DEFAULT_LOCAL_SERVERNAME;
	}

	//Set Local DNS server PTR response.
#if !defined(PLATFORM_MACX)
	if (Parameter.LocalServer_Length == 0)
	{
	//Make PTR response packet.
		auto DNS_Record_PTR = (pdns_record_ptr)Parameter.LocalServer_Response;
		DNS_Record_PTR->PTR = htons(DNS_POINTER_QUERY);
		DNS_Record_PTR->Classes = htons(DNS_CLASS_IN);
		DNS_Record_PTR->TTL = htonl(Parameter.HostsDefaultTTL);
		DNS_Record_PTR->Type = htons(DNS_RECORD_PTR);
		DNS_Record_PTR->Length = htons((uint16_t)Parameter.LocalFQDN_Length);
		Parameter.LocalServer_Length += sizeof(dns_record_ptr);

	//Copy to global buffer.
		memcpy_s(Parameter.LocalServer_Response + Parameter.LocalServer_Length, DOMAIN_MAXSIZE + sizeof(dns_record_ptr) + sizeof(dns_record_opt) - Parameter.LocalServer_Length, Parameter.LocalFQDN_Response, Parameter.LocalFQDN_Length);
		Parameter.LocalServer_Length += Parameter.LocalFQDN_Length;

/* Old version(2015-07-19)
	//EDNS Label
		if (Parameter.EDNS_Label)
		{
			auto DNS_Record_OPT = (pdns_record_opt)(Parameter.LocalServer_Response + Parameter.LocalServer_Length);
			DNS_Record_OPT->Type = htons(DNS_RECORD_OPT);
			DNS_Record_OPT->UDPPayloadSize = htons((uint16_t)Parameter.EDNSPayloadSize);
			Parameter.LocalServer_Length += sizeof(dns_record_opt);

			Parameter.LocalServer_Length = AddEDNSLabelToAdditionalRR(Parameter.LocalServer_Response, Parameter.LocalServer_Length, DOMAIN_MAXSIZE + sizeof(dns_record_ptr) + sizeof(dns_record_opt), true);
		}
*/
	}
#endif

//DNSCurve default settings
#if defined(ENABLE_LIBSODIUM)
	if (Parameter.DNSCurve && DNSCurveParameter.IsEncryption)
	{
	//DNSCurve PayloadSize check
		if (DNSCurveParameter.DNSCurvePayloadSize < DNS_PACKET_MAXSIZE_TRADITIONAL)
		{
			if (DNSCurveParameter.DNSCurvePayloadSize > 0)
				PrintError(LOG_MESSAGE_NOTICE, L"DNSCurve Payload Size must longer than 512 bytes(Traditional DNS packet minimum supported size)", 0, nullptr, 0);
			DNSCurveParameter.DNSCurvePayloadSize = DNS_PACKET_MAXSIZE_TRADITIONAL;
		}
		else if (DNSCurveParameter.DNSCurvePayloadSize >= PACKET_MAXSIZE - sizeof(ipv6_hdr) - sizeof(udp_hdr))
		{
			PrintError(LOG_MESSAGE_NOTICE, L"DNSCurve Payload Size may be too long", 0, nullptr, 0);
			DNSCurveParameter.DNSCurvePayloadSize = EDNS_PACKET_MINSIZE;
		}

	//Main(IPv6)
		if (DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage.ss_family > 0 && 
			CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				memcpy_s(DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN, DNSCRYPT_RECEIVE_MAGIC, DNSCURVE_MAGIC_QUERY_LEN);

	//Main(IPv4)
		if (DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage.ss_family > 0 && 
			CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				memcpy_s(DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN, DNSCRYPT_RECEIVE_MAGIC, DNSCURVE_MAGIC_QUERY_LEN);

	//Alternate(IPv6)
		if (DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.Storage.ss_family > 0 && 
			CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				memcpy_s(DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN, DNSCRYPT_RECEIVE_MAGIC, DNSCURVE_MAGIC_QUERY_LEN);

	//Alternate(IPv4)
		if (DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.Storage.ss_family > 0 && 
			CheckEmptyBuffer(DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN))
				memcpy_s(DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber, DNSCURVE_MAGIC_QUERY_LEN, DNSCRYPT_RECEIVE_MAGIC, DNSCURVE_MAGIC_QUERY_LEN);

	//DNSCurve keys recheck time
		if (DNSCurveParameter.KeyRecheckTime == 0)
			DNSCurveParameter.KeyRecheckTime = DEFAULT_DNSCURVE_RECHECK_TIME * SECOND_TO_MILLISECOND;
	}
#endif

//Sort AcceptTypeList.
	std::sort(Parameter.AcceptTypeList->begin(), Parameter.AcceptTypeList->end());

	return true;
}

//Convert service name to port
uint16_t __fastcall ServiceNameToHex(const char *OriginalBuffer)
{
	std::string Buffer(OriginalBuffer);
	CaseConvert(true, Buffer);

//Server name
	if (Buffer == "TCPMUX")
		return htons(IPPORT_TCPMUX);
	else if (Buffer == "ECHO")
		return htons(IPPORT_ECHO);
	else if (Buffer == "DISCARD")
		return htons(IPPORT_DISCARD);
	else if (Buffer == "SYSTAT")
		return htons(IPPORT_SYSTAT);
	else if (Buffer == "DAYTIME")
		return htons(IPPORT_DAYTIME);
	else if (Buffer == "NETSTAT")
		return htons(IPPORT_NETSTAT);
	else if (Buffer == "QOTD")
		return htons(IPPORT_QOTD);
	else if (Buffer == "MSP")
		return htons(IPPORT_MSP);
	else if (Buffer == "CHARGEN")
		return htons(IPPORT_CHARGEN);
	else if (Buffer == "FTPDATA")
		return htons(IPPORT_FTP_DATA);
	else if (Buffer == "FTP")
		return htons(IPPORT_FTP);
	else if (Buffer == "SSH")
		return htons(IPPORT_SSH);
	else if (Buffer == "TELNET")
		return htons(IPPORT_TELNET);
	else if (Buffer == "SMTP")
		return htons(IPPORT_SMTP);
	else if (Buffer == "TIMESERVER")
		return htons(IPPORT_TIMESERVER);
	else if (Buffer == "RAP")
		return htons(IPPORT_RAP);
	else if (Buffer == "RLP")
		return htons(IPPORT_RLP);
	else if (Buffer == "NAMESERVER")
		return htons(IPPORT_NAMESERVER);
	else if (Buffer == "WHOIS")
		return htons(IPPORT_WHOIS);
	else if (Buffer == "TACACS")
		return htons(IPPORT_TACACS);
	else if (Buffer == "DNS")
		return htons(IPPORT_DNS);
	else if (Buffer == "XNSAUTH")
		return htons(IPPORT_XNSAUTH);
	else if (Buffer == "MTP")
		return htons(IPPORT_MTP);
	else if (Buffer == "BOOTPS")
		return htons(IPPORT_BOOTPS);
	else if (Buffer == "BOOTPC")
		return htons(IPPORT_BOOTPC);
	else if (Buffer == "TFTP")
		return htons(IPPORT_TFTP);
	else if (Buffer == "RJE")
		return htons(IPPORT_RJE);
	else if (Buffer == "FINGER")
		return htons(IPPORT_FINGER);
	else if (Buffer == "HTTP")
		return htons(IPPORT_HTTP);
	else if (Buffer == "HTTPBACKUP")
		return htons(IPPORT_HTTPBACKUP);
	else if (Buffer == "TTYLINK")
		return htons(IPPORT_TTYLINK);
	else if (Buffer == "SUPDUP")
		return htons(IPPORT_SUPDUP);
	else if (Buffer == "POP3")
		return htons(IPPORT_POP3);
	else if (Buffer == "SUNRPC")
		return htons(IPPORT_SUNRPC);
	else if (Buffer == "SQL")
		return htons(IPPORT_SQL);
	else if (Buffer == "NTP")
		return htons(IPPORT_NTP);
	else if (Buffer == "EPMAP")
		return htons(IPPORT_EPMAP);
	else if (Buffer == "NETBIOS_NS")
		return htons(IPPORT_NETBIOS_NS);
	else if (Buffer == "NETBIOS_DGM")
		return htons(IPPORT_NETBIOS_DGM);
	else if (Buffer == "NETBIOS_SSN")
		return htons(IPPORT_NETBIOS_SSN);
	else if (Buffer == "IMAP")
		return htons(IPPORT_IMAP);
	else if (Buffer == "BFTP")
		return htons(IPPORT_BFTP);
	else if (Buffer == "SGMP")
		return htons(IPPORT_SGMP);
	else if (Buffer == "SQLSRV")
		return htons(IPPORT_SQLSRV);
	else if (Buffer == "DMSP")
		return htons(IPPORT_DMSP);
	else if (Buffer == "SNMP")
		return htons(IPPORT_SNMP);
	else if (Buffer == "SNMP_TRAP")
		return htons(IPPORT_SNMP_TRAP);
	else if (Buffer == "ATRTMP")
		return htons(IPPORT_ATRTMP);
	else if (Buffer == "ATHBP")
		return htons(IPPORT_ATHBP);
	else if (Buffer == "QMTP")
		return htons(IPPORT_QMTP);
	else if (Buffer == "IPX")
		return htons(IPPORT_IPX);
	else if (Buffer == "IMAP3")
		return htons(IPPORT_IMAP3);
	else if (Buffer == "BGMP")
		return htons(IPPORT_BGMP);
	else if (Buffer == "TSP")
		return htons(IPPORT_TSP);
	else if (Buffer == "IMMP")
		return htons(IPPORT_IMMP);
	else if (Buffer == "ODMR")
		return htons(IPPORT_ODMR);
	else if (Buffer == "RPC2PORTMAP")
		return htons(IPPORT_RPC2PORTMAP);
	else if (Buffer == "CLEARCASE")
		return htons(IPPORT_CLEARCASE);
	else if (Buffer == "HPALARMMGR")
		return htons(IPPORT_HPALARMMGR);
	else if (Buffer == "ARNS")
		return htons(IPPORT_ARNS);
	else if (Buffer == "AURP")
		return htons(IPPORT_AURP);
	else if (Buffer == "LDAP")
		return htons(IPPORT_LDAP);
	else if (Buffer == "UPS")
		return htons(IPPORT_UPS);
	else if (Buffer == "SLP")
		return htons(IPPORT_SLP);
	else if (Buffer == "HTTPS")
		return htons(IPPORT_HTTPS);
	else if (Buffer == "SNPP")
		return htons(IPPORT_SNPP);
	else if (Buffer == "MICROSOFTDS")
		return htons(IPPORT_MICROSOFT_DS);
	else if (Buffer == "KPASSWD")
		return htons(IPPORT_KPASSWD);
	else if (Buffer == "TCPNETHASPSRV")
		return htons(IPPORT_TCPNETHASPSRV);
	else if (Buffer == "RETROSPECT")
		return htons(IPPORT_RETROSPECT);
	else if (Buffer == "ISAKMP")
		return htons(IPPORT_ISAKMP);
	else if (Buffer == "BIFFUDP")
		return htons(IPPORT_BIFFUDP);
	else if (Buffer == "WHOSERVER")
		return htons(IPPORT_WHOSERVER);
	else if (Buffer == "SYSLOG")
		return htons(IPPORT_SYSLOG);
	else if (Buffer == "ROUTERSERVER")
		return htons(IPPORT_ROUTESERVER);
	else if (Buffer == "NCP")
		return htons(IPPORT_NCP);
	else if (Buffer == "COURIER")
		return htons(IPPORT_COURIER);
	else if (Buffer == "COMMERCE")
		return htons(IPPORT_COMMERCE);
	else if (Buffer == "RTSP")
		return htons(IPPORT_RTSP);
	else if (Buffer == "NNTP")
		return htons(IPPORT_NNTP);
	else if (Buffer == "HTTPRPCEPMAP")
		return htons(IPPORT_HTTPRPCEPMAP);
	else if (Buffer == "IPP")
		return htons(IPPORT_IPP);
	else if (Buffer == "LDAPS")
		return htons(IPPORT_LDAPS);
	else if (Buffer == "MSDP")
		return htons(IPPORT_MSDP);
	else if (Buffer == "AODV")
		return htons(IPPORT_AODV);
	else if (Buffer == "FTPSDATA")
		return htons(IPPORT_FTPSDATA);
	else if (Buffer == "FTPS")
		return htons(IPPORT_FTPS);
	else if (Buffer == "NAS")
		return htons(IPPORT_NAS);
	else if (Buffer == "TELNETS")
		return htons(IPPORT_TELNETS);

//No match.
	return 0;
}

//Convert DNS type name to hex
uint16_t __fastcall DNSTypeNameToHex(const char *OriginalBuffer)
{
	std::string Buffer(OriginalBuffer);
	CaseConvert(true, Buffer);

//DNS type name
	if (Buffer == "A")
		return htons(DNS_RECORD_A);
	else if (Buffer == "NS")
		return htons(DNS_RECORD_NS);
	else if (Buffer == "MD")
		return htons(DNS_RECORD_MD);
	else if (Buffer == "MF")
		return htons(DNS_RECORD_MF);
	else if (Buffer == "CNAME")
		return htons(DNS_RECORD_CNAME);
	else if (Buffer == "SOA")
		return htons(DNS_RECORD_SOA);
	else if (Buffer == "MB")
		return htons(DNS_RECORD_MB);
	else if (Buffer == "MG")
		return htons(DNS_RECORD_MG);
	else if (Buffer == "MR")
		return htons(DNS_RECORD_MR);
	else if (Buffer == "PTR")
		return htons(DNS_RECORD_PTR);
	else if (Buffer == "NULL")
		return htons(DNS_RECORD_NULL);
	else if (Buffer == "WKS")
		return htons(DNS_RECORD_WKS);
	else if (Buffer == "HINFO")
		return htons(DNS_RECORD_HINFO);
	else if (Buffer == "MINFO")
		return htons(DNS_RECORD_MINFO);
	else if (Buffer == "MX")
		return htons(DNS_RECORD_MX);
	else if (Buffer == "TXT")
		return htons(DNS_RECORD_TXT);
	else if (Buffer == "RP")
		return htons(DNS_RECORD_RP);
	else if (Buffer == "SIG")
		return htons(DNS_RECORD_SIG);
	else if (Buffer == "AFSDB")
		return htons(DNS_RECORD_AFSDB);
	else if (Buffer == "X25")
		return htons(DNS_RECORD_X25);
	else if (Buffer == "ISDN")
		return htons(DNS_RECORD_ISDN);
	else if (Buffer == "RT")
		return htons(DNS_RECORD_RT);
	else if (Buffer == "NSAP")
		return htons(DNS_RECORD_NSAP);
	else if (Buffer == "NSAPPTR")
		return htons(DNS_RECORD_NSAP_PTR);
	else if (Buffer == "SIG")
		return htons(DNS_RECORD_SIG);
	else if (Buffer == "KEY")
		return htons(DNS_RECORD_KEY);
	else if (Buffer == "AAAA")
		return htons(DNS_RECORD_AAAA);
	else if (Buffer == "PX")
		return htons(DNS_RECORD_PX);
	else if (Buffer == "GPOS")
		return htons(DNS_RECORD_GPOS);
	else if (Buffer == "LOC")
		return htons(DNS_RECORD_LOC);
	else if (Buffer == "NXT")
		return htons(DNS_RECORD_NXT);
	else if (Buffer == "EID")
		return htons(DNS_RECORD_EID);
	else if (Buffer == "NIMLOC")
		return htons(DNS_RECORD_NIMLOC);
	else if (Buffer == "SRV")
		return htons(DNS_RECORD_SRV);
	else if (Buffer == "ATMA")
		return htons(DNS_RECORD_ATMA);
	else if (Buffer == "NAPTR")
		return htons(DNS_RECORD_NAPTR);
	else if (Buffer == "KX")
		return htons(DNS_RECORD_KX);
	else if (Buffer == "CERT")
		return htons(DNS_RECORD_CERT);
	else if (Buffer == "A6")
		return htons(DNS_RECORD_A6);
	else if (Buffer == "DNAME")
		return htons(DNS_RECORD_DNAME);
	else if (Buffer == "SINK")
		return htons(DNS_RECORD_SINK);
	else if (Buffer == "OPT")
		return htons(DNS_RECORD_OPT);
	else if (Buffer == "APL")
		return htons(DNS_RECORD_APL);
	else if (Buffer == "DS")
		return htons(DNS_RECORD_DS);
	else if (Buffer == "SSHFP")
		return htons(DNS_RECORD_SSHFP);
	else if (Buffer == "IPSECKEY")
		return htons(DNS_RECORD_IPSECKEY);
	else if (Buffer == "RRSIG")
		return htons(DNS_RECORD_RRSIG);
	else if (Buffer == "NSEC")
		return htons(DNS_RECORD_NSEC);
	else if (Buffer == "DNSKEY")
		return htons(DNS_RECORD_DNSKEY);
	else if (Buffer == "DHCID")
		return htons(DNS_RECORD_DHCID);
	else if (Buffer == "NSEC3")
		return htons(DNS_RECORD_NSEC3);
	else if (Buffer == "NSEC3PARAM")
		return htons(DNS_RECORD_NSEC3PARAM);
	else if (Buffer == "TLSA")
		return htons(DNS_RECORD_TLSA);
	else if (Buffer == "HIP")
		return htons(DNS_RECORD_HIP);
	else if (Buffer == "HINFO")
		return htons(DNS_RECORD_HINFO);
	else if (Buffer == "RKEY")
		return htons(DNS_RECORD_RKEY);
	else if (Buffer == "TALINK")
		return htons(DNS_RECORD_TALINK);
	else if (Buffer == "CDS")
		return htons(DNS_RECORD_CDS);
	else if (Buffer == "CDNSKEY")
		return htons(DNS_RECORD_CDNSKEY);
	else if (Buffer == "OPENPGPKEY")
		return htons(DNS_RECORD_OPENPGPKEY);
	else if (Buffer == "SPF")
		return htons(DNS_RECORD_SPF);
	else if (Buffer == "UINFO")
		return htons(DNS_RECORD_UINFO);
	else if (Buffer == "UID")
		return htons(DNS_RECORD_UID);
	else if (Buffer == "GID")
		return htons(DNS_RECORD_GID);
	else if (Buffer == "UNSPEC")
		return htons(DNS_RECORD_UNSPEC);
	else if (Buffer == "NID")
		return htons(DNS_RECORD_NID);
	else if (Buffer == "L32")
		return htons(DNS_RECORD_L32);
	else if (Buffer == "L64")
		return htons(DNS_RECORD_L64);
	else if (Buffer == "LP")
		return htons(DNS_RECORD_LP);
	else if (Buffer == "EUI48")
		return htons(DNS_RECORD_EUI48);
	else if (Buffer == "EUI64")
		return htons(DNS_RECORD_EUI64);
	else if (Buffer == "TKEY")
		return htons(DNS_RECORD_TKEY);
	else if (Buffer == "TSIG")
		return htons(DNS_RECORD_TSIG);
	else if (Buffer == "IXFR")
		return htons(DNS_RECORD_IXFR);
	else if (Buffer == "AXFR")
		return htons(DNS_RECORD_AXFR);
	else if (Buffer == "MAILB")
		return htons(DNS_RECORD_MAILB);
	else if (Buffer == "MAILA")
		return htons(DNS_RECORD_MAILA);
	else if (Buffer == "ANY")
		return htons(DNS_RECORD_ANY);
	else if (Buffer == "URI")
		return htons(DNS_RECORD_URI);
	else if (Buffer == "CAA")
		return htons(DNS_RECORD_CAA);
	else if (Buffer == "TA")
		return htons(DNS_RECORD_TA);
	else if (Buffer == "DLV")
		return htons(DNS_RECORD_DLV);
	else if (Buffer == "RESERVED")
		return htons(DNS_RECORD_RESERVED);

//No match.
	return 0;
}

//Read parameter data from files
bool __fastcall ReadParameterData(std::string Data, const size_t FileIndex, const size_t Line, bool &IsLabelComments)
{
//Delete delete spaces, horizontal tab/HT, check comments(Number Sign/NS and double slashs) and check minimum length of ipfilter items.
//Delete comments(Number Sign/NS and double slashs) and check minimum length of configuration items.
	if (Data.find(ASCII_HASHTAG) == 0 || Data.find(ASCII_SLASH) == 0)
		return true;
	while (Data.find(ASCII_HT) != std::string::npos)
		Data.erase(Data.find(ASCII_HT), 1U);
	while (Data.find(ASCII_SPACE) != std::string::npos)
		Data.erase(Data.find(ASCII_SPACE), 1U);
	if (Data.find(ASCII_HASHTAG) != std::string::npos)
		Data.erase(Data.find(ASCII_HASHTAG), Data.length() - Data.find(ASCII_HASHTAG));
	if (Data.find("//") != std::string::npos)
		Data.erase(Data.find("//"), Data.length() - Data.find("//"));
	if (Data.length() < READ_PARAMETER_MINSIZE)
		return true;

//Multi-line comments check
	if (!ReadMultiLineComments(Data, IsLabelComments))
		return true;
	SSIZE_T Result = 0;

//[Base] block
	if (Data.find("Version=") == 0)
	{
		if (Data.length() > strlen("Version=") && Data.length() < strlen("Version=") + 8U)
		{
			Parameter.Version = strtod(Data.c_str() + strlen("Version="), nullptr);
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

//Parameter version less than 0.4 compatible support
	if (Parameter.Version <= CONFIG_VERSION_POINT_THREE)
	{
	//[Base] block
		if (Data.find("Hosts=") == 0 && Data.length() > strlen("Hosts="))
		{
			if (Data.length() < strlen("Hosts=") + UINT16_MAX_STRING_LENGTH)
			{
				Result = strtoul(Data.c_str() + strlen("Hosts="), nullptr, 0);
				if (errno != ERANGE && Result > 0 && Result >= SHORTEST_FILEREFRESH_TIME)
					Parameter.FileRefreshTime = Result * SECOND_TO_MILLISECOND;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else if (Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family == 0 && Data.find("IPv4DNSAddress=") == 0 && Data.length() > strlen("IPv4DNSAddress="))
		{
			if (Data.length() > strlen("IPv4DNSAddress=") + 6U && Data.length() < strlen("IPv4DNSAddress=") + 20U)
			{
			//Convert IPv4 address and port.
				std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + strlen("IPv4DNSAddress="), Data.length() - strlen("IPv4DNSAddress="));
				if (!AddressStringToBinary(Target.get(), &Parameter.DNSTarget.IPv4.AddressData.IPv4.sin_addr, AF_INET, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

				Parameter.DNSTarget.IPv4.AddressData.IPv4.sin_port = htons(IPPORT_DNS);
				Parameter.DNSTarget.IPv4.AddressData.Storage.ss_family = AF_INET;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else if (Parameter.DNSTarget.Local_IPv4.AddressData.Storage.ss_family == 0 && Data.find("IPv4LocalDNSAddress=") == 0 && Data.length() > strlen("IPv4LocalDNSAddress="))
		{
			if (Data.length() > strlen("IPv4LocalDNSAddress=") + 6U && Data.length() < strlen("IPv4LocalDNSAddress=") + 20U)
			{
			//Convert IPv4 address and port.
				std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + strlen("IPv4LocalDNSAddress="), Data.length() - strlen("IPv4LocalDNSAddress="));
				if (!AddressStringToBinary(Target.get(), &Parameter.DNSTarget.Local_IPv4.AddressData.IPv4.sin_addr, AF_INET, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

				Parameter.DNSTarget.Local_IPv4.AddressData.IPv4.sin_port = htons(IPPORT_DNS);
				Parameter.DNSTarget.Local_IPv4.AddressData.Storage.ss_family = AF_INET;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else if (Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family == 0 && Data.find("IPv6DNSAddress=") == 0 && Data.length() > strlen("IPv6DNSAddress="))
		{
			if (Data.length() > strlen("IPv6DNSAddress=") + 1U && Data.length() < strlen("IPv6DNSAddress=") + 40U)
			{
			//Convert IPv6 address and port.
				std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + strlen("IPv6DNSAddress="), Data.length() - strlen("IPv6DNSAddress="));
				if (!AddressStringToBinary(Target.get(), &Parameter.DNSTarget.IPv6.AddressData.IPv6.sin6_addr, AF_INET6, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

				Parameter.DNSTarget.IPv6.AddressData.IPv6.sin6_port = htons(IPPORT_DNS);
				Parameter.DNSTarget.IPv6.AddressData.Storage.ss_family = AF_INET6;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else if (Parameter.DNSTarget.Local_IPv6.AddressData.Storage.ss_family == 0 && Data.find("IPv6LocalDNSAddress=") == 0 && Data.length() > strlen("IPv6LocalDNSAddress="))
		{
			if (Data.length() > strlen("IPv6LocalDNSAddress=") + 1U && Data.length() < strlen("IPv6LocalDNSAddress=") + 40U)
			{
			//Convert IPv6 address and port.
				std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + strlen("IPv6LocalDNSAddress="), Data.length() - strlen("IPv6LocalDNSAddress="));
				if (!AddressStringToBinary(Target.get(), &Parameter.DNSTarget.Local_IPv6.AddressData.IPv6.sin6_addr, AF_INET6, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

				Parameter.DNSTarget.Local_IPv6.AddressData.IPv6.sin6_port = htons(IPPORT_DNS);
				Parameter.DNSTarget.Local_IPv6.AddressData.Storage.ss_family = AF_INET6;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
	#if defined(ENABLE_PCAP)
		else if (Data.find("HopLimits/TTLFluctuation=") == 0 && Data.length() > strlen("HopLimits/TTLFluctuation="))
		{
			if (Data.length() < strlen("HopLimits/TTLFluctuation=") + 4U)
			{
				Result = strtoul(Data.c_str() + strlen("HopLimits/TTLFluctuation="), nullptr, 0);
				if (errno != ERANGE && Result > 0 && Result < UINT8_MAX)
					Parameter.HopLimitFluctuation = (uint8_t)Result;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
	#endif

	//[Extend Test] block
	#if defined(ENABLE_PCAP)
		else if (Data.find("IPv4OptionsFilter=1") == 0)
		{
			Parameter.HeaderCheck_IPv4 = true;
		}
		else if (Data.find("TCPOptionsFilter=1") == 0)
		{
			Parameter.HeaderCheck_TCP = true;
		}
	#endif
		else if (Data.find("DNSOptionsFilter=1") == 0)
		{
			Parameter.DNSDataCheck = true;
		}

	//[Data] block
	#if defined(ENABLE_PCAP)
		else if (Parameter.DomainTest_Speed == 0 && Data.find("DomainTestSpeed=") == 0 && Data.length() > strlen("DomainTestSpeed="))
		{
			if (Data.length() < strlen("DomainTestSpeed=") + UINT16_MAX_STRING_LENGTH)
			{
				Result = strtoul(Data.c_str() + strlen("DomainTestSpeed="), nullptr, 0);
				if (errno != ERANGE && Result > 0)
					Parameter.DomainTest_Speed = Result * SECOND_TO_MILLISECOND;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
	#endif
	}
	else if (Data.find("FileRefreshTime=") == 0 && Data.length() > strlen("FileRefreshTime="))
	{
		if (Data.length() < strlen("FileRefreshTime=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("FileRefreshTime="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result >= SHORTEST_FILEREFRESH_TIME)
				Parameter.FileRefreshTime = Result * SECOND_TO_MILLISECOND;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("BufferQueueLimits=") == 0 && Data.length() > strlen("BufferQueueLimits="))
	{
		if (Data.length() < strlen("BufferQueueLimits=") + UINT32_MAX_STRING_LENGTH - 1U)
		{
			Result = strtoul(Data.c_str() + strlen("BufferQueueLimits="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result >= BUFFER_QUEUE_MINNUM && Result <= BUFFER_QUEUE_MAXNUM)
				Parameter.BufferQueueSize = Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("QueueLimitsResetTime=") == 0 && Data.length() > strlen("QueueLimitsResetTime="))
	{
		if (Data.length() < strlen("QueueLimitsResetTime=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("QueueLimitsResetTime="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.QueueResetTime = Result * SECOND_TO_MILLISECOND;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("AdditionalPath=") == 0 && Data.length() > strlen("AdditionalPath="))
	{
/* Old version(2015-08-03)
		std::string NameStringTemp;
		std::wstring wNameStringTemp;
		for (Result = strlen("AdditionalPath=");Result < (SSIZE_T)Data.length();++Result)
		{
			if (Result + 1 == (SSIZE_T)Data.length())
			{
				NameStringTemp.append(Data, Result, 1U);

			//Case-insensitive on Windows
			#if defined(PLATFORM_WIN)
				CaseConvert(false, NameStringTemp);
			#endif

			//Add backslash.
				if (NameStringTemp.back() != ASCII_BACKSLASH)
					NameStringTemp.append("\\");

			//Convert to wide string.
				if (!MBSToWCSString(wNameStringTemp, NameStringTemp.c_str()))
				{
					PrintError(LOG_ERROR_PARAMETER, L"Read file path error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				for (auto wStringIter = Parameter.Path_Global->begin();wStringIter < Parameter.Path_Global->end();++wStringIter)
				{
					if (*wStringIter == wNameStringTemp)
						break;

					if (wStringIter + 1U == Parameter.Path_Global->end())
					{
						Parameter.Path_Global->push_back(wNameStringTemp);
						for (size_t Index = 0;Index < Parameter.Path_Global->back().length();++Index)
						{
							if ((Parameter.Path_Global->back())[Index] == L'\\')
							{
								Parameter.Path_Global->back().insert(Index, L"\\");
								++Index;
							}
						}

						break;
					}
				}
			}
			else if (Data.at(Result) == ASCII_VERTICAL)
			{
			//Case-insensitive on Windows
			#if defined(PLATFORM_WIN)
				CaseConvert(false, NameStringTemp);
			#endif

			//Add backslash.
				if (NameStringTemp.back() != ASCII_BACKSLASH)
					NameStringTemp.append("\\");

			//Convert to wide string.
				if (!MBSToWCSString(wNameStringTemp, NameStringTemp.c_str()))
				{
					PrintError(LOG_ERROR_PARAMETER, L"Read file path error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				for (auto wStringIter = Parameter.Path_Global->begin();wStringIter < Parameter.Path_Global->end();++wStringIter)
				{
					if (*wStringIter == wNameStringTemp)
						break;

					if (wStringIter + 1U == Parameter.Path_Global->end())
					{
						Parameter.Path_Global->push_back(wNameStringTemp);
						for (size_t Index = 0;Index < Parameter.Path_Global->back().length();++Index)
						{
							if ((Parameter.Path_Global->back())[Index] == L'\\')
							{
								Parameter.Path_Global->back().insert(Index, L"\\");
								++Index;
							}

							break;
						}
					}
				}

				NameStringTemp.clear();
				wNameStringTemp.clear();
			}
			else {
				NameStringTemp.append(Data, Result, 1U);
			}
		}
*/
	#if defined(PLATFORM_WIN)
		if (!ReadPathAndFileName(Data, strlen("AdditionalPath="), true, Parameter.Path_Global, FileIndex, Line))
	#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
		if (!ReadPathAndFileName(Data, strlen("AdditionalPath="), true, Parameter.Path_Global, Parameter.sPath_Global, FileIndex, Line))
	#endif
			return false;
	}
	else if (Data.find("HostsFileName=") == 0 && Data.length() > strlen("HostsFileName="))
	{
	#if defined(PLATFORM_WIN)
		if (!ReadPathAndFileName(Data, strlen("HostsFileName="), false, Parameter.FileList_Hosts, FileIndex, Line))
	#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
		if (!ReadPathAndFileName(Data, strlen("HostsFileName="), false, Parameter.FileList_Hosts, Parameter.sFileList_Hosts, FileIndex, Line))
	#endif
			return false;
	}
	else if (Data.find("IPFilterFileName=") == 0 && Data.length() > strlen("IPFilterFileName="))
	{
	#if defined(PLATFORM_WIN)
		if (!ReadPathAndFileName(Data, strlen("IPFilterFileName="), false, Parameter.FileList_IPFilter, FileIndex, Line))
	#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
		if (!ReadPathAndFileName(Data, strlen("IPFilterFileName="), false, Parameter.FileList_IPFilter, Parameter.sFileList_IPFilter, FileIndex, Line))
	#endif
			return false;
	}

//[Log] block
	else if (Data.find("PrintError=0") == 0)
	{
		Parameter.PrintError = false;
		delete Parameter.Path_ErrorLog;
		Parameter.Path_ErrorLog = nullptr;
	}
	else if (Data.find("LogMaximumSize=") == 0 && Data.length() > strlen("LogMaximumSize="))
	{
		CaseConvert(true, Data);
		if (Data.find("KB") != std::string::npos)
		{
			Data.erase(Data.length() - 2U, 2U);

		//Mark bytes.
			Result = strtoul(Data.c_str() + strlen("LogMaximumSize="), nullptr, 0);
			if (errno != ERANGE && Result >= 0)
			{
				Parameter.LogMaxSize = Result * KILOBYTE_TIMES;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else if (Data.find("MB") != std::string::npos)
		{
			Data.erase(Data.length() - 2U, 2U);

		//Mark bytes.
			Result = strtoul(Data.c_str() + strlen("LogMaximumSize="), nullptr, 0);
			if (errno != ERANGE && Result >= 0)
			{
				Parameter.LogMaxSize = Result * MEGABYTE_TIMES;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else if (Data.find("GB") != std::string::npos)
		{
			Data.erase(Data.length() - 2U, 2U);

		//Mark bytes.
			Result = strtoul(Data.c_str() + strlen("LogMaximumSize="), nullptr, 0);
			if (errno != ERANGE && Result >= 0)
			{
				Parameter.LogMaxSize = Result * GIGABYTE_TIMES;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else {
		//Check number.
			for (auto StringIter = Data.begin() + strlen("LogMaximumSize=");StringIter != Data.end();++StringIter)
			{
				if (*StringIter < ASCII_ZERO || *StringIter > ASCII_NINE)
				{
					PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
			}

		//Mark bytes.
			Result = strtoul(Data.c_str() + strlen("LogMaximumSize="), nullptr, 0);
			if (errno != ERANGE && Result >= 0)
			{
				Parameter.LogMaxSize = Result;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
	}

//[DNS] block
	else if (Data.find("Protocol=") == 0 && Data.length() > strlen("Protocol="))
	{
		CaseConvert(true, Data);
		if (Data.find("IPV6") != std::string::npos)
		{
			if (Data.find("IPV4") != std::string::npos)
				Parameter.RequestMode_Network = REQUEST_MODE_NETWORK_BOTH;
			else
				Parameter.RequestMode_Network = REQUEST_MODE_IPV6;
		}
		else {
			Parameter.RequestMode_Network = REQUEST_MODE_IPV4;
		}

		if (Data.find("TCP") != std::string::npos)
			Parameter.RequestMode_Transport = REQUEST_MODE_TCP;
		else 
			Parameter.RequestMode_Transport = REQUEST_MODE_UDP;
	}
	else if (Data.find("DirectRequest=") == 0 && Data.length() > strlen("DirectRequest=") || 
		Data.find("HostsOnly=") == 0 && Data.length() > strlen("HostsOnly="))
	{
		if (Data.find("DirectRequest=1") == 0 || Data.find("HostsOnly=1") == 0)
		{
			Parameter.DirectRequest = DIRECT_REQUEST_MODE_BOTH;
		}
		else {
			CaseConvert(true, Data);
			if (Data.find("IPV6") != std::string::npos)
			{
				if (Data.find("IPV4") != std::string::npos)
					Parameter.DirectRequest = DIRECT_REQUEST_MODE_BOTH;
				else 
					Parameter.DirectRequest = DIRECT_REQUEST_MODE_IPV6;
			}
			else if (Data.find("IPV4") != std::string::npos)
			{
				if (Data.find("IPV6") != std::string::npos)
					Parameter.DirectRequest = DIRECT_REQUEST_MODE_BOTH;
				else 
					Parameter.DirectRequest = DIRECT_REQUEST_MODE_IPV4;
			}
		}
	}
	else if (Data.find("LocalMain=1") == 0)
	{
		Parameter.LocalMain = true;
	}
	else if (Data.find("LocalHosts=1") == 0)
	{
		Parameter.LocalHosts = true;
	}
	else if (Data.find("LocalRouting=1") == 0)
	{
		Parameter.LocalRouting = true;
	}
	else if (Data.find("CacheType=") == 0 && Data.length() > strlen("CacheType="))
	{
		CaseConvert(true, Data);
		if (Data.find("TIMER") != std::string::npos)
			Parameter.CacheType = CACHE_TYPE_TIMER;
		else if (Data.find("QUEUE") != std::string::npos)
			Parameter.CacheType = CACHE_TYPE_QUEUE;
	}
	else if (Parameter.CacheType > 0 && Data.find("CacheParameter=") == 0 && Data.length() > strlen("CacheParameter="))
	{
		Result = strtoul(Data.c_str() + strlen("CacheParameter="), nullptr, 0);
		if (errno != ERANGE && Result > 0)
		{
			if (Parameter.CacheType == CACHE_TYPE_TIMER)
				Parameter.CacheParameter = Result * SECOND_TO_MILLISECOND;
			else if (Parameter.CacheType == CACHE_TYPE_QUEUE)
				Parameter.CacheParameter = Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("DefaultTTL=") == 0 && Data.length() > strlen("DefaultTTL="))
	{
		if (Data.length() < strlen("DefaultTTL=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("DefaultTTL="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
			{
				Parameter.HostsDefaultTTL = (uint32_t)Result;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"Default TTL error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

//[Listen] block
#if defined(ENABLE_PCAP)
	else if (Data.find("PcapCapture=1") == 0)
	{
		Parameter.PcapCapture = true;
	}
	else if (Data.find("PcapDevicesBlacklist=") == 0)
	{
/* Old version(2015-08-02)
		std::string NameString;
		for (size_t Index = strlen("PcapDevicesBlacklist=");Index < Data.length();++Index)
		{
		//Last data
			if (Index + 1U == Data.length())
			{
				NameString.append(Data, Index, 1U);
				CaseConvert(false, NameString);
				Parameter.PcapDevicesBlacklist->push_back(NameString);

				break;
			}
		//Separated
			else if (Data.at(Index) == ASCII_VERTICAL)
			{
				CaseConvert(false, NameString);
				Parameter.PcapDevicesBlacklist->push_back(NameString);

				NameString.clear();
			}
			else {
				NameString.append(Data, Index, 1U);
			}
		}
*/
		GetParameterListData(*Parameter.PcapDevicesBlacklist, Data, strlen("PcapDevicesBlacklist="), Data.length());
	}
	else if (Data.find("PcapReadingTimeout=") == 0)
	{
		if (Data.length() < strlen("PcapReadingTimeout=") + UINT32_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("PcapReadingTimeout="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result > PCAP_CAPTURE_MIN_TIMEOUT)
				Parameter.PcapReadingTimeout = (size_t)Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
#endif
	else if (Data.find("ListenProtocol=") == 0 && Data.length() > strlen("ListenProtocol="))
	{
		CaseConvert(true, Data);
		if (Data.find("IPV6") != std::string::npos)
		{
			if (Data.find("IPV4") != std::string::npos)
				Parameter.ListenProtocol_Network = LISTEN_PROTOCOL_NETWORK_BOTH;
			else 
				Parameter.ListenProtocol_Network = LISTEN_PROTOCOL_IPV6;
		}
		else {
			Parameter.ListenProtocol_Network = LISTEN_PROTOCOL_IPV4;
		}

		if (Data.find("TCP") != std::string::npos)
		{
			if (Data.find("UDP") != std::string::npos)
				Parameter.ListenProtocol_Transport = LISTEN_PROTOCOL_TRANSPORT_BOTH;
			else 
				Parameter.ListenProtocol_Transport = LISTEN_PROTOCOL_TCP;
		}
		else {
			Parameter.ListenProtocol_Transport = LISTEN_PROTOCOL_UDP;
		}
	}
	else if (Data.find("ListenPort=") == 0 && Data.length() > strlen("ListenPort="))
	{
/* Old version(2015-08-02)
	//Multiple Ports
		if (Data.find(ASCII_VERTICAL) != std::string::npos)
		{
			std::string PortString;
			for (size_t Index = strlen("ListenPort=");Index < Data.length();++Index)
			{
				if (Index + 1U == Data.length())
				{
					PortString.append(Data, Index, 1U);
					Result = ServiceNameToHex(PortString.c_str());
					if (Result == 0)
					{
						Result = strtoul(PortString.c_str(), nullptr, 0);
						if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
						{
							Parameter.ListenPort->push_back(htons((uint16_t)Result));
						}
						else {
							PrintError(LOG_ERROR_PARAMETER, L"Localhost listening port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
							return false;
						}
					}

					break;
				}
				else if (Data.at(Index) == ASCII_VERTICAL)
				{
					Result = ServiceNameToHex(PortString.c_str());
					if (Result == 0)
					{
						Result = strtoul(PortString.c_str(), nullptr, 0);
						if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
						{
							Parameter.ListenPort->push_back(htons((uint16_t)Result));
						}
						else {
							PrintError(LOG_ERROR_PARAMETER, L"Localhost listening port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
							return false;
						}
					}

					PortString.clear();
				}
				else {
					PortString.append(Data, Index, 1U);
				}
			}
		}
	//Single Ports
		else {
			Result = ServiceNameToHex(Data.c_str() + strlen("ListenPort="));
			if (Result == 0)
			{
				Result = strtoul(Data.c_str() + strlen("ListenPort="), nullptr, 0);
				if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
				{
					Parameter.ListenPort->push_back(htons((uint16_t)Result));
				}
				else {
					PrintError(LOG_ERROR_PARAMETER, L"Localhost listening port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
			}
		}
*/
		std::vector<std::string> ListData;
		GetParameterListData(ListData, Data, strlen("ListenPort="), Data.length());
		for (auto StringIter:ListData)
		{
			Result = ServiceNameToHex(StringIter.c_str());
			if (Result == 0)
			{
				Result = strtoul(StringIter.c_str(), nullptr, 0);
				if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
				{
					Parameter.ListenPort->push_back(htons((uint16_t)Result));
				}
				else {
					PrintError(LOG_ERROR_PARAMETER, L"Localhost listening port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
			}
			else {
				Parameter.ListenPort->push_back(htons((uint16_t)Result));
			}
		}
	}
	else if (Data.find("OperationMode=") == 0 && Data.length() > strlen("OperationMode="))
	{
		CaseConvert(true, Data);
		if (Data.find("PRIVATE") != std::string::npos)
			Parameter.OperationMode = LISTEN_MODE_PRIVATE;
		else if (Data.find("SERVER") != std::string::npos)
			Parameter.OperationMode = LISTEN_MODE_SERVER;
		else if (Data.find("CUSTOM") != std::string::npos)
			Parameter.OperationMode = LISTEN_MODE_CUSTOM;
		else 
			Parameter.OperationMode = LISTEN_MODE_PROXY;
	}
	else if (Data.find("IPFilterType=PERMIT") == 0 || Data.find("IPFilterType=Permit") == 0 || Data.find("IPFilterType=permit") == 0)
	{
		Parameter.IPFilterType = true;
	}
	else if (Data.find("IPFilterLevel<") == 0 && Data.length() > strlen("IPFilterLevel<"))
	{
		if (Data.length() < strlen("IPFilterLevel<") + 4U)
		{
			Result = strtoul(Data.c_str() + strlen("IPFilterLevel<"), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
			{
				Parameter.IPFilterLevel = (size_t)Result;
			}
			else {
				PrintError(LOG_ERROR_PARAMETER, L"IPFilter Level error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("AcceptType=") == 0 && Data.length() > strlen("AcceptType="))
	{
		if (Data.find(ASCII_COLON) == std::string::npos)
		{
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
		else {
		//Permit or Deny
			if (Data.find("Permit:") != std::string::npos || Data.find("PERMIT:") != std::string::npos || Data.find("permit:") != std::string::npos)
				Parameter.AcceptType = true;
			else 
				Parameter.AcceptType = false;

/* Old version(2015-08-02)
			std::string TypeString(Data, Data.find(ASCII_COLON) + 1U);
		//Add to global list.
			if (TypeString.empty())
			{
				PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

			else if (TypeString.find(ASCII_COMMA) == std::string::npos && TypeString.find(ASCII_VERTICAL) == std::string::npos)
			{
				Result = DNSTypeNameToHex(TypeString.c_str());
				if (Result == 0)
				{
				//Number types
					Result = strtoul(TypeString.c_str(), nullptr, 0);
					if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
					{
						Parameter.AcceptTypeList->push_back(htons((uint16_t)Result));
					}
					else {
						PrintError(LOG_ERROR_PARAMETER, L"DNS Records type error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
				}
				else {
					Parameter.AcceptTypeList->push_back((uint16_t)Result);
				}
			}
			else {
				std::string TypeStringTemp;
				Result = 0;
				for (size_t Index = 0;Index < TypeString.length();++Index)
				{
				//Last data
					if (Index + 1U == TypeString.length())
					{
						TypeStringTemp.append(TypeString, Result, (SSIZE_T)Index - Result + 1U);
						Result = DNSTypeNameToHex(TypeStringTemp.c_str());
						if (Result == 0) 
						{
						//Number types
							Result = strtoul(TypeStringTemp.c_str(), nullptr, 0);
							if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
							{
								Parameter.AcceptTypeList->push_back(htons((uint16_t)Result));
							}
							else {
								PrintError(LOG_ERROR_PARAMETER, L"DNS Records type error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
								return false;
							}
						}
						else {
							Parameter.AcceptTypeList->push_back((uint16_t)Result);
						}
					}
				//Separated
					else if (TypeString.at(Index) == ASCII_COMMA || TypeString.at(Index) == ASCII_VERTICAL)
					{
						TypeStringTemp.append(TypeString, Result, (SSIZE_T)Index - Result);
						Result = DNSTypeNameToHex(TypeStringTemp.c_str());
						if (Result == 0)
						{
						//Number types
							Result = strtoul(TypeStringTemp.c_str(), nullptr, 0);
							if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
							{
								Parameter.AcceptTypeList->push_back(htons((uint16_t)Result));
							}
							else {
								PrintError(LOG_ERROR_PARAMETER, L"DNS Records type error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
								return false;
							}
						}
						else {
							Parameter.AcceptTypeList->push_back((uint16_t)Result);
						}

						TypeStringTemp.clear();
						Result = Index + 1U;
					}
				}
*/
			std::vector<std::string> ListData;
			GetParameterListData(ListData, Data, Data.find(ASCII_COLON) + 1U, Data.length());
			for (auto StringIter:ListData)
			{
				Result = DNSTypeNameToHex(StringIter.c_str());
				if (Result == 0)
				{
					Result = strtoul(StringIter.c_str(), nullptr, 0);
					if (errno != ERANGE && Result > 0 && Result <= UINT16_MAX)
					{
						Parameter.AcceptTypeList->push_back(htons((uint16_t)Result));
					}
					else {
						PrintError(LOG_ERROR_PARAMETER, L"DNS Records type error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
				}
				else {
					Parameter.AcceptTypeList->push_back((uint16_t)Result);
				}
			}
		}
	}

//[Addresses] block
	else if (Data.find("IPv4ListenAddress=") == 0 && Data.length() > strlen("IPv4ListenAddress="))
	{
		std::shared_ptr<sockaddr_storage> SockAddr(new sockaddr_storage());
		memset(SockAddr.get(), 0, sizeof(sockaddr_storage));
		if (!ReadMultipleAddresses(Data, strlen("IPv4ListenAddress="), *SockAddr, Parameter.ListenAddress_IPv4, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4EDNSClientSubnetAddress=") == 0 && Data.length() > strlen("IPv4EDNSClientSubnetAddress="))
	{
		if (!ReadAddressPrefixBlock(Data, strlen("IPv4EDNSClientSubnetAddress="), FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4DNSAddress=") == 0 && Data.length() > strlen("IPv4DNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv4DNSAddress="), Parameter.DNSTarget.IPv4.AddressData.Storage, nullptr, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4AlternateDNSAddress=") == 0 && Data.length() > strlen("IPv4AlternateDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv4AlternateDNSAddress="), Parameter.DNSTarget.Alternate_IPv4.AddressData.Storage, nullptr, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4LocalDNSAddress=") == 0 && Data.length() > strlen("IPv4LocalDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv4LocalDNSAddress="), Parameter.DNSTarget.Local_IPv4.AddressData.Storage, nullptr, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4LocalAlternateDNSAddress=") == 0 && Data.length() > strlen("IPv4LocalAlternateDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv4LocalAlternateDNSAddress="), Parameter.DNSTarget.Alternate_Local_IPv4.AddressData.Storage, nullptr, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6ListenAddress=") == 0 && Data.length() > strlen("IPv6ListenAddress="))
	{
		std::shared_ptr<sockaddr_storage> SockAddr(new sockaddr_storage());
		memset(SockAddr.get(), 0, sizeof(sockaddr_storage));
		if (!ReadMultipleAddresses(Data, strlen("IPv4ListenAddress="), *SockAddr, Parameter.ListenAddress_IPv6, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6EDNSClientSubnetAddress=") == 0 && Data.length() > strlen("IPv6EDNSClientSubnetAddress="))
	{
		if (!ReadAddressPrefixBlock(Data, strlen("IPv6EDNSClientSubnetAddress="), FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6DNSAddress=") == 0 && Data.length() > strlen("IPv6DNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv6DNSAddress="), Parameter.DNSTarget.IPv6.AddressData.Storage, nullptr, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6AlternateDNSAddress=") == 0 && Data.length() > strlen("IPv6AlternateDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv6AlternateDNSAddress="), Parameter.DNSTarget.Alternate_IPv6.AddressData.Storage, nullptr, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6LocalDNSAddress=") == 0 && Data.length() > strlen("IPv6LocalDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv6LocalDNSAddress="), Parameter.DNSTarget.Local_IPv6.AddressData.Storage, nullptr, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6LocalAlternateDNSAddress=") == 0 && Data.length() > strlen("IPv6LocalAlternateDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("IPv6LocalAlternateDNSAddress="), Parameter.DNSTarget.Alternate_Local_IPv6.AddressData.Storage, nullptr, AF_INET6, FileIndex, Line))
			return false;
	}

//[Values] block
	else if (Data.find("EDNSPayloadSize=") == 0 && Data.length() > strlen("EDNSPayloadSize="))
	{
		if (Data.length() < strlen("EDNSPayloadSize=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("EDNSPayloadSize="), nullptr, 0);
			if (errno != ERANGE && Result >= 0)
				Parameter.EDNSPayloadSize = Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
#if defined(ENABLE_PCAP)
	else if (Data.find("IPv4TTL=") == 0 && Data.length() > strlen("IPv4TTL="))
	{
		if (!ReadHopLimitData(Data, strlen("IPv4TTL="), Parameter.DNSTarget.IPv4.HopLimitData.TTL, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6HopLimits=") == 0 && Data.length() > strlen("IPv6HopLimits="))
	{
		if (!ReadHopLimitData(Data, strlen("IPv6HopLimits="), Parameter.DNSTarget.IPv6.HopLimitData.HopLimit, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4AlternateTTL=") == 0 && Data.length() > strlen("IPv4AlternateTTL="))
	{
		if (!ReadHopLimitData(Data, strlen("IPv4AlternateTTL="), Parameter.DNSTarget.Alternate_IPv4.HopLimitData.TTL, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6AlternateHopLimits=") == 0 && Data.length() > strlen("IPv6AlternateHopLimits="))
	{
		if (!ReadHopLimitData(Data, strlen("IPv6AlternateHopLimits="), Parameter.DNSTarget.Alternate_IPv6.HopLimitData.HopLimit, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("HopLimitsFluctuation=") == 0 && Data.length() > strlen("HopLimitsFluctuation="))
	{
		if (Data.length() < strlen("HopLimitsFluctuation=") + 4U)
		{
			Result = strtoul(Data.c_str() + strlen("HopLimitsFluctuation="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result < UINT8_MAX)
				Parameter.HopLimitFluctuation = (uint8_t)Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
#endif
	else if (Data.find("ReliableSocketTimeout=") == 0 && Data.length() > strlen("ReliableSocketTimeout="))
	{
		if (Data.length() < strlen("ReliableSocketTimeout=") + UINT32_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("ReliableSocketTimeout="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result > SOCKET_MIN_TIMEOUT)
			#if defined(PLATFORM_WIN)
				Parameter.SocketTimeout_Reliable = (int)Result;
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			{
				Parameter.SocketTimeout_Reliable.tv_sec = Result / SECOND_TO_MILLISECOND;
				Parameter.SocketTimeout_Reliable.tv_usec = Result % SECOND_TO_MILLISECOND * MICROSECOND_TO_MILLISECOND;
			}
			#endif
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("UnreliableSocketTimeout=") == 0 && Data.length() > strlen("UnreliableSocketTimeout="))
	{
		if (Data.length() < strlen("UnreliableSocketTimeout=") + UINT32_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("UnreliableSocketTimeout="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result > SOCKET_MIN_TIMEOUT)
			#if defined(PLATFORM_WIN)
				Parameter.SocketTimeout_Unreliable = (int)Result;
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			{
				Parameter.SocketTimeout_Unreliable.tv_sec = Result / SECOND_TO_MILLISECOND;
				Parameter.SocketTimeout_Unreliable.tv_usec = Result % SECOND_TO_MILLISECOND * MICROSECOND_TO_MILLISECOND;
			}
			#endif
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("ReceiveWaiting=") == 0 && Data.length() > strlen("ReceiveWaiting="))
	{
		if (Data.length() < strlen("ReceiveWaiting=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("ReceiveWaiting="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.ReceiveWaiting = (size_t)Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
#if defined(ENABLE_PCAP)
	else if (Data.find("ICMPTest=") == 0 && Data.length() > strlen("ICMPTest="))
	{
		if (Data.length() < strlen("ICMPTest=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("ICMPTest="), nullptr, 0);
			if (errno != ERANGE && Result >= 5)
				Parameter.ICMP_Speed = Result * SECOND_TO_MILLISECOND;
			else if (Result > 0 && Result < DEFAULT_ICMPTEST_TIME)
				Parameter.ICMP_Speed = DEFAULT_ICMPTEST_TIME * SECOND_TO_MILLISECOND;
			else 
				Parameter.ICMP_Speed = 0; //ICMP Test Disable.
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("DomainTest=") == 0 && Data.length() > strlen("DomainTest="))
	{
		if (Data.length() < strlen("DomainTest=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("DomainTest="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.DomainTest_Speed = Result * SECOND_TO_MILLISECOND;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);

			return false;
		}
	}
#endif
	else if (Data.find("AlternateTimes=") == 0 && Data.length() > strlen("AlternateTimes="))
	{
		if (Data.length() < strlen("AlternateTimes=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("AlternateTimes="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.AlternateTimes = Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("AlternateTimeRange=") == 0 && Data.length() > strlen("AlternateTimeRange="))
	{
		if (Data.length() < strlen("AlternateTimeRange=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("AlternateTimeRange="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result >= DEFAULT_ALTERNATE_RANGE)
				Parameter.AlternateTimeRange = Result * SECOND_TO_MILLISECOND;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("AlternateResetTime=") == 0 && Data.length() > strlen("AlternateResetTime="))
	{
		if (Data.length() < strlen("AlternateResetTime=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("AlternateResetTime="), nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result >= DEFAULT_ALTERNATE_RESET_TIME)
				Parameter.AlternateResetTime = Result * SECOND_TO_MILLISECOND;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("MultiRequestTimes=") == 0 && Data.length() > strlen("MultiRequestTimes="))
	{
		if (Data.length() < strlen("MultiRequestTimes=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("MultiRequestTimes="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.MultiRequestTimes = Result + 1U;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

//[Switches] block
	else if (Data.find("TCPFastOpen=1") == 0)
	{
		Parameter.TCP_FastOpen = true;
	}
	else if (Data.find("DomainCaseConversion=1") == 0)
	{
		Parameter.DomainCaseConversion = true;
	}
	else if (Data.find("CompressionPointerMutation=") == 0 && Data.length() > strlen("CompressionPointerMutation="))
	{
		if (Data.find(ASCII_ONE) != std::string::npos)
			Parameter.CPM_PointerToHeader = true;
		if (Data.find(ASCII_TWO) != std::string::npos)
			Parameter.CPM_PointerToRR = true;
		if (Data.find(ASCII_THREE) != std::string::npos)
			Parameter.CPM_PointerToAdditional = true;
		if (Parameter.CPM_PointerToHeader || Parameter.CPM_PointerToRR || Parameter.CPM_PointerToAdditional)
			Parameter.CompressionPointerMutation = true;
	}
	else if (Data.find("EDNSLabel=1") == 0 || Data.find("EDNS0Label=1") == 0) //EDNS0 Label has changed to EDNS Label.
	{
		Parameter.EDNS_Label = true;
	}
	else if (Data.find("EDNSClientSubnet=1") == 0)
	{
		Parameter.EDNS_ClientSubnet = true;
	}
	else if (Data.find("DNSSECRequest=1") == 0)
	{
		Parameter.DNSSEC_Request = true;
	}
	else if (Data.find("DNSSECValidation=1") == 0)
	{
		Parameter.DNSSEC_Validation = true;
	}
	else if (Data.find("DNSSECForceValidation=1") == 0)
	{
		Parameter.DNSSEC_ForceValidation = true;
	}
	else if (Data.find("AlternateMultiRequest=1") == 0)
	{
		Parameter.AlternateMultiRequest = true;
	}
#if defined(ENABLE_PCAP)
	else if (Data.find("IPv4DataFilter=1") == 0)
	{
		Parameter.HeaderCheck_IPv4 = true;
	}
	else if (Data.find("TCPDataFilter=1") == 0)
	{
		Parameter.HeaderCheck_TCP = true;
	}
#endif
	else if (Data.find("DNSDataFilter=1") == 0)
	{
		Parameter.DNSDataCheck = true;
	}
	else if (Data.find("BlacklistFilter=1") == 0)
	{
		Parameter.BlacklistCheck = true;
	}

//[Data] block
#if defined(ENABLE_PCAP)
	else if (Data.find("ICMPID=") == 0 && Data.length() > strlen("ICMPID="))
	{
		if (Data.length() < strlen("ICMPID=") + 7U)
		{
			Result = strtoul(Data.c_str() + strlen("ICMPID="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.ICMP_ID = htons((uint16_t)Result);
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("ICMPSequence=") == 0 && Data.length() > strlen("ICMPSequence="))
	{
		if (Data.length() < strlen("ICMPSequence=") + 7U)
		{
			Result = strtoul(Data.c_str() + strlen("ICMPSequence="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.ICMP_Sequence = htons((uint16_t)Result);
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("ICMPPaddingData=") == 0 && Data.length() > strlen("ICMPPaddingData="))
	{
		if (Data.length() > strlen("ICMPPaddingData=") + 17U && Data.length() < strlen("ICMPPaddingData=") + ICMP_PADDING_MAXSIZE - 1U)
		{
			Parameter.ICMP_PaddingLength = Data.length() - strlen("ICMPPaddingData=") - 1U;
			memcpy_s(Parameter.ICMP_PaddingData, ICMP_PADDING_MAXSIZE, Data.c_str() + strlen("ICMPPaddingData="), Data.length() - strlen("ICMPPaddingData="));
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("DomainTestID=") == 0 && Data.length() > strlen("DomainTestID="))
	{
		if (Data.length() < strlen("DomainTestID=") + 7U)
		{
			Result = strtoul(Data.c_str() + strlen("DomainTestID="), nullptr, 0);
			if (errno != ERANGE && Result > 0)
				Parameter.DomainTest_ID = htons((uint16_t)Result);
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("DomainTestData=") == 0 && Data.length() > strlen("DomainTestData="))
	{
		if (Data.length() > strlen("DomainTestData=") + DOMAIN_MINSIZE && Data.length() < strlen("DomainTestData=") + DOMAIN_DATA_MAXSIZE)
		{
			memcpy_s(Parameter.DomainTest_Data, DOMAIN_MAXSIZE, Data.c_str() + strlen("DomainTestData="), Data.length() - strlen("DomainTestData="));
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
#endif
	else if (Data.find("LocalhostServerName=") == 0 && Data.length() > strlen("LocalhostServerName="))
	{
		if (Data.length() > strlen("LocalhostServerName=") + DOMAIN_MINSIZE && Data.length() < strlen("LocalhostServerName=") + DOMAIN_DATA_MAXSIZE)
		{
			std::shared_ptr<char> LocalFQDN(new char[DOMAIN_MAXSIZE]());
			memset(LocalFQDN.get(), 0, DOMAIN_MAXSIZE);
			Parameter.LocalFQDN_Length = Data.length() - strlen("LocalhostServerName=");
			memcpy_s(LocalFQDN.get(), DOMAIN_MAXSIZE, Data.c_str() + strlen("LocalhostServerName="), Parameter.LocalFQDN_Length);
			*Parameter.LocalFQDN_String = LocalFQDN.get();
			Result = CharToDNSQuery(LocalFQDN.get(), Parameter.LocalFQDN_Response);
			if (Result > DOMAIN_MINSIZE)
			{
				Parameter.LocalFQDN_Length = Result;
			}
			else {
				Parameter.LocalFQDN_Length = 0;
				memset(Parameter.LocalFQDN_Response, 0, DOMAIN_MAXSIZE);
				Parameter.LocalFQDN_String->clear();
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

//[DNSCurve] block
#if defined(ENABLE_LIBSODIUM)
	else if (Data.find("DNSCurve=1") == 0)
	{
		Parameter.DNSCurve = true;
	}
	else if (Data.find("DNSCurveProtocol=TCP") == 0 || Data.find("DNSCurveProtocol=Tcp") == 0 || Data.find("DNSCurveProtocol=tcp") == 0)
	{
		DNSCurveParameter.DNSCurveMode = DNSCURVE_REQUEST_MODE_TCP;
	}
	else if (Data.find("DNSCurvePayloadSize=") == 0 && Data.length() > strlen("DNSCurvePayloadSize="))
	{
		if (Data.length() > strlen("DNSCurvePayloadSize=") + 2U)
		{
			Result = strtoul(Data.c_str() + strlen("DNSCurvePayloadSize="), nullptr, 0);
			if (errno != ERANGE && Result > (SSIZE_T)(sizeof(eth_hdr) + sizeof(ipv4_hdr) + sizeof(udp_hdr) + sizeof(uint16_t) + DNSCURVE_MAGIC_QUERY_LEN + crypto_box_PUBLICKEYBYTES + crypto_box_HALF_NONCEBYTES))
				DNSCurveParameter.DNSCurvePayloadSize = Result;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else if (Data.find("Encryption=1") == 0)
	{
		DNSCurveParameter.IsEncryption = true;
	}
	else if (Data.find("EncryptionOnly=1") == 0)
	{
		DNSCurveParameter.IsEncryptionOnly = true;
	}
	else if (Data.find("KeyRecheckTime=") == 0 && Data.length() > strlen("KeyRecheckTime="))
	{
		if (Data.length() < strlen("KeyRecheckTime=") + UINT16_MAX_STRING_LENGTH)
		{
			Result = strtoul(Data.c_str() + strlen("KeyRecheckTime="), nullptr, 0);
			if (Result >= SHORTEST_DNSCURVE_RECHECK_TIME && Result < DEFAULT_DNSCURVE_RECHECK_TIME)
				DNSCurveParameter.KeyRecheckTime = Result * SECOND_TO_MILLISECOND;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

//[DNSCurve Addresses] block
	else if (Data.find("DNSCurveIPv4DNSAddress=") == 0 && Data.length() > strlen("DNSCurveIPv4DNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("DNSCurveIPv4DNSAddress="), DNSCurveParameter.DNSCurveTarget.IPv4.AddressData.Storage, nullptr, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv4AlternateDNSAddress=") == 0 && Data.length() > strlen("DNSCurveIPv4AlternateDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("DNSCurveIPv4AlternateDNSAddress="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.AddressData.Storage, nullptr, AF_INET, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv6DNSAddress=") == 0 && Data.length() > strlen("DNSCurveIPv6DNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("DNSCurveIPv6DNSAddress="), DNSCurveParameter.DNSCurveTarget.IPv6.AddressData.Storage, nullptr, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv6AlternateDNSAddress=") == 0 && Data.length() > strlen("DNSCurveIPv6AlternateDNSAddress="))
	{
		if (!ReadMultipleAddresses(Data, strlen("DNSCurveIPv6AlternateDNSAddress="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.AddressData.Storage, nullptr, AF_INET6, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv4ProviderName=") == 0 && Data.length() > strlen("DNSCurveIPv4ProviderName="))
	{
		if (!ReadDNSCurveProviderName(Data, strlen("DNSCurveIPv4ProviderName="), DNSCurveParameter.DNSCurveTarget.IPv4.ProviderName, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv4AlternateProviderName=") == 0 && Data.length() > strlen("DNSCurveIPv4AlternateProviderName="))
	{
		if (!ReadDNSCurveProviderName(Data, strlen("DNSCurveIPv4AlternateProviderName="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ProviderName, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv6ProviderName=") == 0 && Data.length() > strlen("DNSCurveIPv6ProviderName="))
	{
		if (!ReadDNSCurveProviderName(Data, strlen("DNSCurveIPv6ProviderName="), DNSCurveParameter.DNSCurveTarget.IPv6.ProviderName, FileIndex, Line))
			return false;
	}
	else if (Data.find("DNSCurveIPv6AlternateProviderName=") == 0 && Data.length() > strlen("DNSCurveIPv6AlternateProviderName="))
	{
		if (!ReadDNSCurveProviderName(Data, strlen("DNSCurveIPv6AlternateProviderName="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ProviderName, FileIndex, Line))
			return false;
	}

//[DNSCurve Keys] block
	else if (Data.find("ClientPublicKey=") == 0 && Data.length() > strlen("ClientPublicKey="))
	{
		if (!ReadDNSCurveKey(Data, strlen("ClientPublicKey="), DNSCurveParameter.Client_PublicKey, FileIndex, Line))
			return false;
	}
	else if (Data.find("ClientSecretKey=") == 0 && Data.length() > strlen("ClientSecretKey="))
	{
		if (!ReadDNSCurveKey(Data, strlen("ClientSecretKey="), DNSCurveParameter.Client_SecretKey, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4DNSPublicKey=") == 0 && Data.length() > strlen("IPv4DNSPublicKey="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv4DNSPublicKey="), DNSCurveParameter.DNSCurveTarget.IPv4.ServerPublicKey, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4AlternateDNSPublicKey=") == 0 && Data.length() > strlen("IPv4AlternateDNSPublicKey="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv4AlternateDNSPublicKey="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerPublicKey, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6DNSPublicKey=") == 0 && Data.length() > strlen("IPv6DNSPublicKey="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv6DNSPublicKey="), DNSCurveParameter.DNSCurveTarget.IPv6.ServerPublicKey, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6AlternateDNSPublicKey=") == 0 && Data.length() > strlen("IPv6AlternateDNSPublicKey="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv6AlternateDNSPublicKey="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerPublicKey, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4DNSFingerprint=") == 0 && Data.length() > strlen("IPv4DNSFingerprint="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv4DNSFingerprint="), DNSCurveParameter.DNSCurveTarget.IPv4.ServerFingerprint, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4AlternateDNSFingerprint=") == 0 && Data.length() > strlen("IPv4AlternateDNSFingerprint="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv4AlternateDNSFingerprint="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ServerFingerprint, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6DNSFingerprint=") == 0 && Data.length() > strlen("IPv6DNSFingerprint="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv6DNSFingerprint="), DNSCurveParameter.DNSCurveTarget.IPv6.ServerFingerprint, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6AlternateDNSFingerprint=") == 0 && Data.length() > strlen("IPv6AlternateDNSFingerprint="))
	{
		if (!ReadDNSCurveKey(Data, strlen("IPv6AlternateDNSFingerprint="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ServerFingerprint, FileIndex, Line))
			return false;
	}

//[DNSCurve Magic Number] block
	else if (Data.find("IPv4ReceiveMagicNumber=") == 0 && Data.length() > strlen("IPv4ReceiveMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv4ReceiveMagicNumber="), DNSCurveParameter.DNSCurveTarget.IPv4.ReceiveMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4AlternateReceiveMagicNumber=") == 0 && Data.length() > strlen("IPv4AlternateReceiveMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv4AlternateReceiveMagicNumber="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.ReceiveMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6ReceiveMagicNumber=") == 0 && Data.length() > strlen("IPv6ReceiveMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv6ReceiveMagicNumber="), DNSCurveParameter.DNSCurveTarget.IPv6.ReceiveMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6AlternateReceiveMagicNumber=") == 0 && Data.length() > strlen("IPv6AlternateReceiveMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv6AlternateReceiveMagicNumber="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.ReceiveMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4DNSMagicNumber=") == 0 && Data.length() > strlen("IPv4DNSMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv4DNSMagicNumber="), DNSCurveParameter.DNSCurveTarget.IPv4.SendMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv4AlternateDNSMagicNumber=") == 0 && Data.length() > strlen("IPv4AlternateDNSMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv4AlternateDNSMagicNumber="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv4.SendMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6DNSMagicNumber=") == 0 && Data.length() > strlen("IPv6DNSMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv6DNSMagicNumber="), DNSCurveParameter.DNSCurveTarget.IPv6.SendMagicNumber, FileIndex, Line))
			return false;
	}
	else if (Data.find("IPv6AlternateDNSMagicNumber=") == 0 && Data.length() > strlen("IPv6AlternateDNSMagicNumber="))
	{
		if (!ReadMagicNumber(Data, strlen("IPv6AlternateDNSMagicNumber="), DNSCurveParameter.DNSCurveTarget.Alternate_IPv6.SendMagicNumber, FileIndex, Line))
			return false;
	}
#endif

	return true;
}

//Read file names from data
#if defined(PLATFORM_WIN)
	bool __fastcall ReadPathAndFileName(std::string Data, const size_t DataOffset, const bool Path, std::vector<std::wstring> *ListData, const size_t FileIndex, const size_t Line)
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
	bool ReadPathAndFileName(std::string Data, const size_t DataOffset, const bool Path, std::vector<std::wstring> *ListData, std::vector<std::string> *sListData, const size_t FileIndex, const size_t Line)
#endif
{
/* Old version(2015-08-03)
//Initialization
	std::string NameStringTemp;
	std::wstring wNameStringTemp;

//Read file name.
	for (SSIZE_T Result = DataOffset;Result < (SSIZE_T)Data.length();++Result)
	{
		if (Result + 1 == (SSIZE_T)Data.length())
		{
			NameStringTemp.append(Data, Result, 1U);
		//Case-insensitive on Windows
		#if defined(PLATFORM_WIN)
			CaseConvert(false, NameStringTemp);
		#endif

		//Add to global list.
			if (!MBSToWCSString(wNameStringTemp, NameStringTemp.c_str()))
			{
				PrintError(LOG_ERROR_PARAMETER, L"Read file name error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
			if (ListData->empty())
			{
				ListData->push_back(wNameStringTemp);
			}
			else {
				for (auto IPFilterFileTableIter = ListData->begin();IPFilterFileTableIter < ListData->end();++IPFilterFileTableIter)
				{
					if (*IPFilterFileTableIter == wNameStringTemp)
						break;

					if (IPFilterFileTableIter + 1U == ListData->end())
					{
						ListData->push_back(wNameStringTemp);
						break;
					}
				}
			}

		#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			if (sListData->empty())
			{
				sListData->push_back(NameStringTemp);
			}
			else {
				for (auto IPFilterFileTableIter = sListData->begin();IPFilterFileTableIter < sListData->end();++IPFilterFileTableIter)
				{
					if (*IPFilterFileTableIter == NameStringTemp)
						break;

					if (IPFilterFileTableIter + 1U == sListData->end())
					{
						sListData->push_back(NameStringTemp);
						break;
					}
				}
			}
		#endif
		}
		else if (Data.at(Result) == ASCII_VERTICAL)
		{
		//Case-insensitive on Windows
		#if defined(PLATFORM_WIN)
			CaseConvert(false, NameStringTemp);
		#endif

		//Add to global list.
			if (!MBSToWCSString(wNameStringTemp, NameStringTemp.c_str()))
			{
				PrintError(LOG_ERROR_PARAMETER, L"Read file name error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}
			if (ListData->empty())
			{
				ListData->push_back(wNameStringTemp);
			}
			else {
				for (auto IPFilterFileTableIter = ListData->begin();IPFilterFileTableIter < ListData->end();++IPFilterFileTableIter)
				{
					if (*IPFilterFileTableIter == wNameStringTemp)
						break;

					if (IPFilterFileTableIter + 1U == ListData->end())
					{
						ListData->push_back(wNameStringTemp);
						break;
					}
				}
			}

		#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			if (sListData->empty())
			{
				sListData->push_back(NameStringTemp);
			}
			else {
				for (auto IPFilterFileTableIter = sListData->begin();IPFilterFileTableIter < sListData->end();++IPFilterFileTableIter)
				{
					if (*IPFilterFileTableIter == NameStringTemp)
						break;

					if (IPFilterFileTableIter + 1U == sListData->end())
					{
						sListData->push_back(NameStringTemp);
						break;
					}
				}
			}
		#endif

			NameStringTemp.clear();
			wNameStringTemp.clear();
		}
		else {
			NameStringTemp.append(Data, Result, 1U);
		}
	}
*/
//Initialization
	std::vector<std::string> InnerListData;
	std::wstring wNameString;
	GetParameterListData(InnerListData, Data, DataOffset, Data.length());

//Read file path.
	if (Path)
	{
	//Mark all data in list.
		for (auto StringIter:InnerListData)
		{
		//Case-insensitive on Windows
		#if defined(PLATFORM_WIN)
			CaseConvert(false, StringIter);
		#endif

		//Add backslash.
			if (StringIter.back() != ASCII_BACKSLASH)
				StringIter.append("\\");

		//Convert to wide string.
			wNameString.clear();
			if (!MBSToWCSString(wNameString, StringIter.c_str()))
			{
				PrintError(LOG_ERROR_PARAMETER, L"Read file path error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Double backslash
			for (size_t Index = 0;Index < wNameString.length();++Index)
			{
				if (wNameString.at(Index) == L'\\')
				{
					wNameString.insert(Index, L"\\");
					++Index;
				}
			}

		//Add to global list.
			for (auto InnerStringIter = Parameter.Path_Global->begin();InnerStringIter < Parameter.Path_Global->end();++InnerStringIter)
			{
				if (*InnerStringIter == wNameString)
				{
					break;
				}
				else if (InnerStringIter + 1U == Parameter.Path_Global->end())
				{
					Parameter.Path_Global->push_back(wNameString);
					break;
				}
			}

		#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			for (auto InnerStringIter = Parameter.sPath_Global->begin();InnerStringIter < Parameter.sPath_Global->end();++InnerStringIter)
			{
				if (*InnerStringIter == StringIter)
				{
					break;
				}
				else if (InnerStringIter + 1U == Parameter.sPath_Global->end())
				{
					Parameter.sPath_Global->push_back(StringIter);
					break;
				}
			}
		#endif
		}
	}
//Read file name.
	else {
	//Mark all data in list.
		for (auto StringIter:InnerListData)
		{
		//Case-insensitive on Windows
		#if defined(PLATFORM_WIN)
			CaseConvert(false, StringIter);
		#endif

		//Convert to wide string.
			wNameString.clear();
			if (!MBSToWCSString(wNameString, StringIter.c_str()))
			{
				PrintError(LOG_ERROR_PARAMETER, L"Read file path error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Add to global list.
			if (ListData->empty())
			{
				ListData->push_back(wNameString);
			}
			else {
				for (auto InnerStringIter = ListData->begin();InnerStringIter != ListData->end();++InnerStringIter)
				{
					if (*InnerStringIter == wNameString)
					{
						break;
					}
					else if (InnerStringIter + 1U == ListData->end())
					{
						ListData->push_back(wNameString);
						break;
					}
				}
			}

		#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			if (sListData->empty())
			{
				sListData->push_back(StringIter);
			}
			else {
				for (auto InnerStringIter = sListData->begin();InnerStringIter != sListData->end();++InnerStringIter)
				{
					if (*InnerStringIter == StringIter)
					{
						break;
					}
					else if (InnerStringIter + 1U == sListData->end())
					{
						sListData->push_back(StringIter);
						break;
					}
				}
			}
		#endif
		}
	}

	return true;
}

/* Old version(2015-08-03)
//Read listen address
bool __fastcall ReadListenAddress(std::string Data, const size_t DataOffset, const uint16_t Protocol, const size_t FileIndex, const size_t Line)
{
	SSIZE_T Result = 0;

//IPv6
	if (Protocol == AF_INET6)
	{
		if (Data.length() > DataOffset + 6U && (Data.length() < DataOffset + 48U || Data.find(ASCII_VERTICAL) != std::string::npos))
		{
		//IPv6 address and port check.
			if (Data.find(ASCII_BRACKETS_LEAD) == std::string::npos || Data.find(ASCII_BRACKETS_TRAIL) == std::string::npos || 
				Data.find(ASCII_BRACKETS_TRAIL) < DataOffset + IPV6_SHORTEST_ADDRSTRING)
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

			std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			std::shared_ptr<sockaddr_storage> SockAddr(new sockaddr_storage());
			memset(SockAddr.get(), 0, sizeof(sockaddr_storage));

		//Multi requesting
			if (Data.find(ASCII_VERTICAL) != std::string::npos)
			{
				Data.erase(0, DataOffset);

			//Delete all front brackets and port colon.
				while (Data.find(ASCII_BRACKETS_LEAD) != std::string::npos)
					Data.erase(Data.find(ASCII_BRACKETS_LEAD), 1U);
				while (Data.find("]:") != std::string::npos)
					Data.erase(Data.find("]:") + 1U, strlen(":"));

			//Read data.
				while (Data.find(ASCII_BRACKETS_TRAIL) != std::string::npos && Data.find(ASCII_VERTICAL) != std::string::npos && Data.find(ASCII_BRACKETS_TRAIL) < Data.find(ASCII_VERTICAL))
				{
				//Convert IPv6 address.
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_BRACKETS_TRAIL));
					if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN6)SockAddr.get())->sin6_addr, AF_INET6, Result))
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}

				//Convert IPv6 port.
					Data.erase(0, Data.find(ASCII_BRACKETS_TRAIL) + 1U);
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_VERTICAL));
					Result = ServiceNameToHex(Target.get());
					if (Result == 0)
					{
						Result = strtoul(Target.get(), nullptr, 0);
						if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
						{
							PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
							return false;
						}
						else {
							((PSOCKADDR_IN6)SockAddr.get())->sin6_port = htons((uint16_t)Result);
						}
					}
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

				//Add to global list.
					SockAddr->ss_family = AF_INET6;
					Parameter.ListenAddress_IPv6->push_back(*SockAddr);
					memset(SockAddr.get(), 0, sizeof(sockaddr_storage));
					Data.erase(0, Data.find(ASCII_VERTICAL) + 1U);
				}

			//Last data
				//Convert IPv6 address.
				if (Data.find(ASCII_BRACKETS_TRAIL) != std::string::npos)
				{
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_BRACKETS_TRAIL));
				}
				else {
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN6)SockAddr.get())->sin6_addr, AF_INET6, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

				//Convert IPv6 port.
				Data.erase(0, Data.find(ASCII_BRACKETS_TRAIL) + 1U);
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.length());
				Result = ServiceNameToHex(Target.get());
				if (Result == 0)
				{
					Result = strtoul(Target.get(), nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						((PSOCKADDR_IN6)SockAddr.get())->sin6_port = htons((uint16_t)Result);
					}
				}

			//Add to global list.
				SockAddr->ss_family = AF_INET6;
				Parameter.ListenAddress_IPv6->push_back(*SockAddr);
			}
			else {
			//Convert IPv6 address.
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset + 1U, Data.find(ASCII_BRACKETS_TRAIL) - DataOffset - 1U);
				if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN6)SockAddr.get())->sin6_addr, AF_INET6, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

			//Convert IPv6 port.
				Result = ServiceNameToHex(Data.c_str() + Data.find(ASCII_BRACKETS_TRAIL) + 2U);
				if (Result == 0)
				{
					Result = strtoul(Data.c_str() + Data.find(ASCII_BRACKETS_TRAIL) + 2U, nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						((PSOCKADDR_IN6)SockAddr.get())->sin6_port = htons((uint16_t)Result);
					}
				}

				SockAddr->ss_family = AF_INET6;
				Parameter.ListenAddress_IPv6->push_back(*SockAddr);
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
//IPv4
	else {
		if (Data.length() > DataOffset + 8U && (Data.length() < DataOffset + 22U || Data.find(ASCII_VERTICAL) != std::string::npos))
		{
		//IPv4 address and port check
			if (Data.find(ASCII_COLON) == std::string::npos || Data.find(ASCII_COLON) < DataOffset + IPV4_SHORTEST_ADDRSTRING)
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

			std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			std::shared_ptr<sockaddr_storage> SockAddr(new sockaddr_storage());
			memset(SockAddr.get(), 0, sizeof(sockaddr_storage));

		//Multi requesting.
			if (Data.find(ASCII_VERTICAL) != std::string::npos)
			{
				Data.erase(0, DataOffset);

			//Read data.
				while (Data.find(ASCII_COLON) != std::string::npos && Data.find(ASCII_VERTICAL) != std::string::npos && Data.find(ASCII_COLON) < Data.find(ASCII_VERTICAL))
				{
				//Convert IPv4 address.
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_COLON));
					if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN)SockAddr.get())->sin_addr, AF_INET, Result))
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}

				//Convert IPv4 port.
					Data.erase(0, Data.find(ASCII_COLON) + 1U);
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_VERTICAL));
					Result = ServiceNameToHex(Target.get());
					if (Result == 0)
					{
						Result = strtoul(Target.get(), nullptr, 0);
						if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
						{
							PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
							return false;
						}
						else {
							((PSOCKADDR_IN)SockAddr.get())->sin_port = htons((uint16_t)Result);
						}
					}
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

				//Add to global list.
					SockAddr->ss_family = AF_INET;
					Parameter.ListenAddress_IPv4->push_back(*SockAddr);
					memset(SockAddr.get(), 0, sizeof(sockaddr_storage));
					Data.erase(0, Data.find(ASCII_VERTICAL) + 1U);
				}

			//Last data
				//Convert IPv4 address.
				if (Data.find(ASCII_COLON) != std::string::npos)
				{
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_COLON));
				}
				else {
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN)SockAddr.get())->sin_addr, AF_INET, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

				//Convert IPv4 port.
				Data.erase(0, Data.find(ASCII_COLON) + 1U);
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.length());
				Result = ServiceNameToHex(Target.get());
				if (Result == 0)
				{
					Result = strtoul(Target.get(), nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						((PSOCKADDR_IN)SockAddr.get())->sin_port = htons((uint16_t)Result);
					}
				}
				Target.reset();

			//Add to global list.
				SockAddr->ss_family = AF_INET;
				Parameter.ListenAddress_IPv4->push_back(*SockAddr);
			}
			else {
			//Convert IPv4 address.
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset, Data.find(ASCII_COLON) - DataOffset);
				if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN)SockAddr.get())->sin_addr, AF_INET, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				Target.reset();

			//Convert IPv4 port.
				Result = ServiceNameToHex(Data.c_str() + Data.find(ASCII_COLON) + 1U);
				if (Result == 0)
				{
					Result = strtoul(Data.c_str() + Data.find(ASCII_COLON) + 1U, nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						((PSOCKADDR_IN)SockAddr.get())->sin_port = htons((uint16_t)Result);
					}
				}

				SockAddr->ss_family = AF_INET;
				Parameter.ListenAddress_IPv4->push_back(*SockAddr);
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

	return true;
}

//Read single address from data
bool __fastcall ReadSingleAddress(std::string Data, const size_t DataOffset, sockaddr_storage &SockAddr, const uint16_t Protocol, const size_t FileIndex, const size_t Line)
{
//Initialization
	SSIZE_T Result = 0;
	std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
	memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

//IPv6
	if (Protocol == AF_INET6)
	{
		if (Data.length() > DataOffset + 6U && Data.length() < DataOffset + 48U)
		{
		//IPv6 address and port check.
			if (Data.find(ASCII_BRACKETS_LEAD) == std::string::npos || Data.find(ASCII_BRACKETS_TRAIL) == std::string::npos || 
				Data.find(ASCII_BRACKETS_TRAIL) < DataOffset + IPV6_SHORTEST_ADDRSTRING)
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv6 address.
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset + 1U, Data.find(ASCII_BRACKETS_TRAIL) - DataOffset - 1U);
			if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN6)&SockAddr)->sin6_addr, AF_INET6, Result))
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv6 port.
			Result = ServiceNameToHex(Data.c_str() + Data.find(ASCII_BRACKETS_TRAIL) + 2U);
			if (Result == 0)
			{
				Result = strtoul(Data.c_str() + Data.find(ASCII_BRACKETS_TRAIL) + 2U, nullptr, 0);
				if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				else {
					((PSOCKADDR_IN6)&SockAddr)->sin6_port = htons((uint16_t)Result);
				}
			}

			SockAddr.ss_family = AF_INET6;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
//IPv4
	else {
		if (Data.length() > DataOffset + 8U && Data.length() < DataOffset + 22U)
		{
		//IPv4 address and port check
			if (Data.find(ASCII_COLON) == std::string::npos || Data.find(ASCII_COLON) < DataOffset + IPV4_SHORTEST_ADDRSTRING)
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv4 address.
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset, Data.find(ASCII_COLON) - DataOffset);
			if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN)&SockAddr)->sin_addr, AF_INET, Result))
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv4 port.
			Result = ServiceNameToHex(Data.c_str() + Data.find(ASCII_COLON) + 1U);
			if (Result == 0)
			{
				Result = strtoul(Data.c_str() + Data.find(ASCII_COLON) + 1U, nullptr, 0);
				if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				else {
					((PSOCKADDR_IN)&SockAddr)->sin_port = htons((uint16_t)Result);
				}
			}

			SockAddr.ss_family = AF_INET;
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}

	return true;
}
*/

//Read multiple addresses from data
bool __fastcall ReadMultipleAddresses(std::string Data, const size_t DataOffset, sockaddr_storage &SockAddr, std::vector<sockaddr_storage> *ListenSockAddr, const uint16_t Protocol, const size_t FileIndex, const size_t Line)
{
//Initialization
	std::vector<std::string> ListData;
	std::shared_ptr<DNS_SERVER_DATA> DNSServerDataTemp(new DNS_SERVER_DATA());
	std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
	SSIZE_T Result = 0;
	GetParameterListData(ListData, Data, DataOffset, Data.length());

//IPv6
	if (Protocol == AF_INET6)
	{
/* Old version(2015-08-02)
		if (Data.length() > DataOffset + 6U && (Data.length() < DataOffset + 48U || Data.find(ASCII_VERTICAL) != std::string::npos))
		{
		//IPv6 address and port check.
			if (Data.find(ASCII_BRACKETS_LEAD) == std::string::npos || Data.find(ASCII_BRACKETS_TRAIL) == std::string::npos || 
				Data.find(ASCII_BRACKETS_TRAIL) < DataOffset + IPV6_SHORTEST_ADDRSTRING)
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

			std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
		//Multi requesting
			if (Data.find(ASCII_VERTICAL) != std::string::npos)
			{
				std::shared_ptr<DNS_SERVER_DATA> DNSServerDataTemp(new DNS_SERVER_DATA());
				memset(DNSServerDataTemp.get(), 0, sizeof(DNS_SERVER_DATA));
				Data.erase(0, DataOffset);

			//Delete all front brackets and port colon.
				while (Data.find(ASCII_BRACKETS_LEAD) != std::string::npos)
					Data.erase(Data.find(ASCII_BRACKETS_LEAD), 1U);
				while (Data.find("]:") != std::string::npos)
					Data.erase(Data.find("]:") + 1U, strlen(":"));

			//Read data.
				while (Data.find(ASCII_BRACKETS_TRAIL) != std::string::npos && Data.find(ASCII_VERTICAL) != std::string::npos && Data.find(ASCII_BRACKETS_TRAIL) < Data.find(ASCII_VERTICAL))
				{
				//Convert IPv6 address.
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_BRACKETS_TRAIL));
					if (!AddressStringToBinary(Target.get(), &DNSServerDataTemp->AddressData.IPv6.sin6_addr, AF_INET6, Result))
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}

				//Convert IPv6 port.
					Data.erase(0, Data.find(ASCII_BRACKETS_TRAIL) + 1U);
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_VERTICAL));
					Result = ServiceNameToHex(Target.get());
					if (Result == 0)
					{
						Result = strtoul(Target.get(), nullptr, 0);
						if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
						{
							PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
							return false;
						}
						else {
							DNSServerDataTemp->AddressData.IPv6.sin6_port = htons((uint16_t)Result);
						}
					}
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

				//Add to global list.
					DNSServerDataTemp->AddressData.Storage.ss_family = AF_INET6;
					Parameter.DNSTarget.IPv6_Multi->push_back(*DNSServerDataTemp);
					memset(DNSServerDataTemp.get(), 0, sizeof(DNS_SERVER_DATA));
					Data.erase(0, Data.find(ASCII_VERTICAL) + 1U);
				}

			//Last data
				//Convert IPv6 address.
				if (Data.find(ASCII_BRACKETS_TRAIL) != std::string::npos)
				{
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_BRACKETS_TRAIL));
				}
				else {
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				if (!AddressStringToBinary(Target.get(), &DNSServerDataTemp->AddressData.IPv6.sin6_addr, AF_INET6, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

			//Convert IPv6 port.
				Data.erase(0, Data.find(ASCII_BRACKETS_TRAIL) + 1U);
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.length());
				Result = ServiceNameToHex(Target.get());
				if (Result == 0)
				{
					Result = strtoul(Target.get(), nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						DNSServerDataTemp->AddressData.IPv6.sin6_port = htons((uint16_t)Result);
					}
				}

			//Add to global list.
				DNSServerDataTemp->AddressData.Storage.ss_family = AF_INET6;
				Parameter.DNSTarget.IPv6_Multi->push_back(*DNSServerDataTemp);
			}
			else {
			//Convert IPv6 address.
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset + 1U, Data.find(ASCII_BRACKETS_TRAIL) - DataOffset - 1U);
				if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN6)&SockAddr)->sin6_addr, AF_INET6, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

			//Convert IPv6 port.
				Result = ServiceNameToHex(Data.c_str() + Data.find(ASCII_BRACKETS_TRAIL) + 2U);
				if (Result == 0)
				{
					Result = strtoul(Data.c_str() + Data.find(ASCII_BRACKETS_TRAIL) + 2U, nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						((PSOCKADDR_IN6)&SockAddr)->sin6_port = htons((uint16_t)Result);
					}
				}

				SockAddr.ss_family = AF_INET6;
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
*/
	//Mark all data in list.
		for (auto StringIter:ListData)
		{
			memset(DNSServerDataTemp.get(), 0, sizeof(DNS_SERVER_DATA));
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

		//IPv6 address and port check.
			if (StringIter.find(ASCII_BRACKETS_LEAD) == std::string::npos || StringIter.find(ASCII_BRACKETS_TRAIL) == std::string::npos || 
				StringIter.find("]:") == std::string::npos || StringIter.find(ASCII_BRACKETS_TRAIL) <= strlen("[") || 
				StringIter.find(ASCII_BRACKETS_TRAIL) < IPV6_SHORTEST_ADDRSTRING || StringIter.length() <= StringIter.find("]:") + strlen("]:"))
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv6 address.
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, StringIter.c_str() + strlen("["), StringIter.find(ASCII_BRACKETS_TRAIL) - strlen("["));
			if (!AddressStringToBinary(Target.get(), &DNSServerDataTemp->AddressData.IPv6.sin6_addr, AF_INET6, Result))
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv6 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv6 port.
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, StringIter.c_str() + StringIter.find("]:") + strlen("]:"), StringIter.length() - (StringIter.find("]:") + strlen("]:")));
			Result = ServiceNameToHex(Target.get());
			if (Result == 0)
			{
				Result = strtoul(Target.get(), nullptr, 0);
				if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv6 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				else {
					DNSServerDataTemp->AddressData.IPv6.sin6_port = htons((uint16_t)Result);
				}
			}
			else {
				DNSServerDataTemp->AddressData.IPv6.sin6_port = htons((uint16_t)Result);
			}

		//Add to global list.
			DNSServerDataTemp->AddressData.Storage.ss_family = AF_INET6;
			if (ListenSockAddr != nullptr)
			{
				memcpy_s(&SockAddr, sizeof(sockaddr_storage), &DNSServerDataTemp->AddressData.Storage, sizeof(sockaddr_storage));
				ListenSockAddr->push_back(SockAddr);
			}
			else if (SockAddr.ss_family > 0)
			{
				Parameter.DNSTarget.IPv6_Multi->push_back(*DNSServerDataTemp);
			}
			else {
				memcpy_s(&SockAddr, sizeof(sockaddr_storage), &DNSServerDataTemp->AddressData.Storage, sizeof(sockaddr_storage));
			}
		}
	}
//IPv4
	else {
/* Old version(2015-08-02)
		if (Data.length() > DataOffset + 8U && (Data.length() < DataOffset + 22U || Data.find(ASCII_VERTICAL) != std::string::npos))
		{
		//IPv4 address and port check
			if (Data.find(ASCII_COLON) == std::string::npos || Data.find(ASCII_COLON) < DataOffset + IPV4_SHORTEST_ADDRSTRING)
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

			std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
		//Multi requesting.
			if (Data.find(ASCII_VERTICAL) != std::string::npos)
			{
				std::shared_ptr<DNS_SERVER_DATA> DNSServerDataTemp(new DNS_SERVER_DATA());
				memset(DNSServerDataTemp.get(), 0, sizeof(DNS_SERVER_DATA));
				Data.erase(0, DataOffset);

			//Read data.
				while (Data.find(ASCII_COLON) != std::string::npos && Data.find(ASCII_VERTICAL) != std::string::npos && Data.find(ASCII_COLON) < Data.find(ASCII_VERTICAL))
				{
				//Convert IPv4 address.
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_COLON));
					if (!AddressStringToBinary(Target.get(), &DNSServerDataTemp->AddressData.IPv4.sin_addr, AF_INET, Result))
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}

				//Convert IPv4 port.
					Data.erase(0, Data.find(ASCII_COLON) + 1U);
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_VERTICAL));
					Result = ServiceNameToHex(Target.get());
					if (Result == 0)
					{
						Result = strtoul(Target.get(), nullptr, 0);
						if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
						{
							PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
							return false;
						}
						else {
							DNSServerDataTemp->AddressData.IPv4.sin_port = htons((uint16_t)Result);
						}
					}
					memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

				//Add to global list.
					DNSServerDataTemp->AddressData.Storage.ss_family = AF_INET;
					Parameter.DNSTarget.IPv4_Multi->push_back(*DNSServerDataTemp);
					memset(DNSServerDataTemp.get(), 0, sizeof(DNS_SERVER_DATA));
					Data.erase(0, Data.find(ASCII_VERTICAL) + 1U);
				}

			//Last data
				//Convert IPv4 address.
				if (Data.find(ASCII_COLON) != std::string::npos)
				{
					memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_COLON));
				}
				else {
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				if (!AddressStringToBinary(Target.get(), &DNSServerDataTemp->AddressData.IPv4.sin_addr, AF_INET, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}

			//Convert IPv4 port.
				Data.erase(0, Data.find(ASCII_COLON) + 1U);
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.length());
				Result = ServiceNameToHex(Target.get());
				if (Result == 0)
				{
					Result = strtoul(Target.get(), nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						DNSServerDataTemp->AddressData.IPv4.sin_port = htons((uint16_t)Result);
					}
				}
				Target.reset();

			//Add to global list.
				DNSServerDataTemp->AddressData.Storage.ss_family = AF_INET;
				Parameter.DNSTarget.IPv4_Multi->push_back(*DNSServerDataTemp);
			}
			else {
			//Convert IPv4 address.
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset, Data.find(ASCII_COLON) - DataOffset);
				if (!AddressStringToBinary(Target.get(), &((PSOCKADDR_IN)&SockAddr)->sin_addr, AF_INET, Result))
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", Result, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				Target.reset();

			//Convert IPv4 port.
				Result = ServiceNameToHex(Data.c_str() + Data.find(ASCII_COLON) + 1U);
				if (Result == 0)
				{
					Result = strtoul(Data.c_str() + Data.find(ASCII_COLON) + 1U, nullptr, 0);
					if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
					{
						PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
						return false;
					}
					else {
						((PSOCKADDR_IN)&SockAddr)->sin_port = htons((uint16_t)Result);
					}
				}

				SockAddr.ss_family = AF_INET;
			}
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
*/
	//Mark all data in list.
		for (auto StringIter:ListData)
		{
			memset(DNSServerDataTemp.get(), 0, sizeof(DNS_SERVER_DATA));
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);

		//IPv4 address and port check.
			if (StringIter.find(ASCII_COLON) == std::string::npos || StringIter.find(ASCII_PERIOD) == std::string::npos || 
				StringIter.find(ASCII_COLON) < IPV4_SHORTEST_ADDRSTRING || StringIter.length() <= StringIter.find(ASCII_COLON) + strlen(":"))
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv4 address.
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, StringIter.c_str(), StringIter.find(ASCII_COLON));
			if (!AddressStringToBinary(Target.get(), &DNSServerDataTemp->AddressData.IPv4.sin_addr, AF_INET, Result))
			{
				PrintError(LOG_ERROR_PARAMETER, L"IPv4 address format error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
				return false;
			}

		//Convert IPv4 port.
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, StringIter.c_str() + StringIter.find(ASCII_COLON) + strlen(":"), StringIter.length() - (StringIter.find(ASCII_COLON) + strlen(":")));
			Result = ServiceNameToHex(Target.get());
			if (Result == 0)
			{
				Result = strtoul(Target.get(), nullptr, 0);
				if (errno == ERANGE || Result <= 0 || Result > UINT16_MAX)
				{
					PrintError(LOG_ERROR_PARAMETER, L"IPv4 address port error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				else {
					DNSServerDataTemp->AddressData.IPv4.sin_port = htons((uint16_t)Result);
				}
			}
			else {
				DNSServerDataTemp->AddressData.IPv4.sin_port = htons((uint16_t)Result);
			}

		//Add to global list.
			DNSServerDataTemp->AddressData.Storage.ss_family = AF_INET;
			if (ListenSockAddr != nullptr)
			{
				memcpy_s(&SockAddr, sizeof(sockaddr_storage), &DNSServerDataTemp->AddressData.Storage, sizeof(sockaddr_storage));
				ListenSockAddr->push_back(SockAddr);
			}
			else if (SockAddr.ss_family > 0)
			{
				Parameter.DNSTarget.IPv4_Multi->push_back(*DNSServerDataTemp);
			}
			else {
				memcpy_s(&SockAddr, sizeof(sockaddr_storage), &DNSServerDataTemp->AddressData.Storage, sizeof(sockaddr_storage));
			}
		}
	}

	return true;
}

//Read TTL or HopLimit from data
#if defined(ENABLE_PCAP)
bool __fastcall ReadHopLimitData(std::string Data, const size_t DataOffset, uint8_t &HopLimit, const uint16_t Protocol, const size_t FileIndex, const size_t Line)
{
	if (Data.length() > DataOffset && Data.length() < DataOffset + ADDR_STRING_MAXSIZE)
	{
/* Old version(2015-08-02)
		if (Data.find(ASCII_VERTICAL) == std::string::npos)
		{
			Result = strtoul(Data.c_str() + DataOffset, nullptr, 0);
			if (errno != ERANGE && Result > 0 && Result < UINT8_MAX)
				HopLimit = (uint8_t)Result;
		}
		else {
			std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			Data.erase(0, DataOffset);
			size_t Index = 0;

			while (Data.find(ASCII_VERTICAL) != std::string::npos && Data.find(ASCII_VERTICAL) > 0)
			{
				memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
				memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.find(ASCII_VERTICAL));
				Data.erase(0, Data.find(ASCII_VERTICAL) + 1U);
				Result = strtoul(Target.get(), nullptr, 0);

			//Mark TTL or HopLimit.
				if (Protocol == AF_INET6) //IPv6
				{
					if (errno != ERANGE && Result > 0 && Result < UINT8_MAX && Parameter.DNSTarget.IPv6_Multi->size() > Index)
						Parameter.DNSTarget.IPv6_Multi->at(Index).HopLimitData.TTL = (uint8_t)Result;
				}
				else { //IPv4
					if (errno != ERANGE && Result > 0 && Result < UINT8_MAX && Parameter.DNSTarget.IPv4_Multi->size() > Index)
						Parameter.DNSTarget.IPv4_Multi->at(Index).HopLimitData.TTL = (uint8_t)Result;
				}

				++Index;
			}

		//Last data
			memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
			memcpy_s(Target.get(), ADDR_STRING_MAXSIZE, Data.c_str(), Data.length());
			Result = strtoul(Target.get(), nullptr, 0);
			if (Protocol == AF_INET6) //IPv6
			{
				if (errno != ERANGE && Result > 0 && Result < UINT8_MAX && Parameter.DNSTarget.IPv6_Multi->size() > Index)
					Parameter.DNSTarget.IPv6_Multi->at(Index).HopLimitData.TTL = (uint8_t)Result;
			}
			else { //IPv4
				if (errno != ERANGE && Result > 0 && Result < UINT8_MAX && Parameter.DNSTarget.IPv4_Multi->size() > Index)
					Parameter.DNSTarget.IPv4_Multi->at(Index).HopLimitData.TTL = (uint8_t)Result;
			}
		}
*/
		std::vector<std::string> ListData;
		GetParameterListData(ListData, Data, DataOffset, Data.length());
		SSIZE_T Result = 0;
		size_t Index = 0;

	//Mark all data in list.
		for (auto StringIter:ListData)
		{
			Result = strtoul(StringIter.c_str(), nullptr, 0);
			if (Index == 0)
			{
				if (errno != ERANGE && Result > 0 && Result < UINT8_MAX)
					HopLimit = (uint8_t)Result;
			}
			else {
				if (Protocol == AF_INET6) //IPv6
				{
				//Check queue.
					for (size_t InnerIndex = 0;InnerIndex < Parameter.DNSTarget.IPv6_Multi->size();++InnerIndex)
					{
						if (Parameter.DNSTarget.IPv6_Multi->at(InnerIndex).HopLimitData.HopLimit == 0)
						{
							Index = InnerIndex;
							break;
						}
					}

				//Convert Hop Limit value.
					if (errno != ERANGE && Result > 0 && Result < UINT8_MAX && Parameter.DNSTarget.IPv6_Multi->size() + 1U > Index)
						Parameter.DNSTarget.IPv6_Multi->at(Index - 1U).HopLimitData.HopLimit = (uint8_t)Result;
				}
				else { //IPv4
				//Check queue.
					for (size_t InnerIndex = 0;InnerIndex < Parameter.DNSTarget.IPv4_Multi->size();++InnerIndex)
					{
						if (Parameter.DNSTarget.IPv4_Multi->at(InnerIndex).HopLimitData.TTL == 0)
						{
							Index = InnerIndex;
							break;
						}
					}

				//Convert Hop Limit value.
					if (errno != ERANGE && Result > 0 && Result < UINT8_MAX && Parameter.DNSTarget.IPv4_Multi->size() + 1U > Index)
						Parameter.DNSTarget.IPv4_Multi->at(Index - 1U).HopLimitData.TTL = (uint8_t)Result;
				}
			}

			++Index;
		}
	}
	else {
		PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
		return false;
	}

	return true;
}
#endif

//Read Provider Name of DNSCurve server
#if defined(ENABLE_LIBSODIUM)
bool __fastcall ReadDNSCurveProviderName(std::string Data, const size_t DataOffset, PSTR ProviderNameData, const size_t FileIndex, const size_t Line)
{
	if (Data.length() > DataOffset + DOMAIN_MINSIZE && Data.length() < DataOffset + DOMAIN_DATA_MAXSIZE)
	{
		for (SSIZE_T Result = DataOffset;Result < (SSIZE_T)(Data.length() - DataOffset);++Result)
		{
			for (size_t Index = 0;Index < strnlen_s(Parameter.DomainTable, DOMAIN_MAXSIZE);++Index)
			{
				if (Index == strnlen_s(Parameter.DomainTable, DOMAIN_MAXSIZE) - 1U && Data.at(Result) != Parameter.DomainTable[Index])
				{
					PrintError(LOG_ERROR_PARAMETER, L"DNSCurve Provider Names error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
					return false;
				}
				if (Data.at(Result) == Parameter.DomainTable[Index])
					break;
			}
		}

		memcpy_s(ProviderNameData, DOMAIN_MAXSIZE, Data.c_str() + DataOffset, Data.length() - DataOffset);
	}
	else {
		PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
		return false;
	}

	return true;
}

//Read DNSCurve secret keys, public keys and fingerprints
bool __fastcall ReadDNSCurveKey(std::string Data, const size_t DataOffset, PUINT8 KeyData, const size_t FileIndex, const size_t Line)
{
//Initialization
	std::shared_ptr<char> Target(new char[ADDR_STRING_MAXSIZE]());
	memset(Target.get(), 0, ADDR_STRING_MAXSIZE);
	const char *ResultPointer = nullptr;
	size_t ResultLength = 0;

//Convert hex format to binary.
	if (Data.length() > DataOffset + crypto_box_PUBLICKEYBYTES * 2U && Data.length() < DataOffset + crypto_box_PUBLICKEYBYTES * 3U)
	{
		SSIZE_T Result = sodium_hex2bin((PUCHAR)Target.get(), ADDR_STRING_MAXSIZE, Data.c_str() + DataOffset, Data.length() - DataOffset, ": ", &ResultLength, &ResultPointer);
		if (Result == 0 && ResultLength == crypto_box_PUBLICKEYBYTES && ResultPointer != nullptr)
		{
			memcpy_s(KeyData, crypto_box_SECRETKEYBYTES, Target.get(), crypto_box_PUBLICKEYBYTES);
		}
		else {
			PrintError(LOG_ERROR_PARAMETER, L"DNSCurve Keys error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
	else {
		PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
		return false;
	}

	return true;
}

//Read DNSCurve magic number
bool __fastcall ReadMagicNumber(std::string Data, const size_t DataOffset, PSTR MagicNumber, const size_t FileIndex, const size_t Line)
{
//Hex format
	if (Data.find("0x") == DataOffset && Data.length() == DataOffset + DNSCURVE_MAGIC_QUERY_HEX_LEN + strlen("0x"))
	{
	//Initialization
		const char *ResultPointer = nullptr;
		size_t ResultLength = 0;

	//Convert hex format to binary.
		SSIZE_T Result = sodium_hex2bin((PUCHAR)MagicNumber, DNSCURVE_MAGIC_QUERY_LEN, Data.c_str() + DataOffset + strlen("0x"), DNSCURVE_MAGIC_QUERY_HEX_LEN, nullptr, &ResultLength, &ResultPointer);
		if (Result != 0 || ResultLength != DNSCURVE_MAGIC_QUERY_LEN || ResultPointer == nullptr)
		{
			PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
			return false;
		}
	}
//ASCI format
	else if (Data.length() == DataOffset + DNSCURVE_MAGIC_QUERY_LEN)
	{
		memcpy_s(MagicNumber, DNSCURVE_MAGIC_QUERY_LEN, Data.c_str() + DataOffset, DNSCURVE_MAGIC_QUERY_LEN);
	}
	else {
		PrintError(LOG_ERROR_PARAMETER, L"Data length error", 0, ConfigFileList.at(FileIndex).c_str(), Line);
		return false;
	}

	return true;
}
#endif
