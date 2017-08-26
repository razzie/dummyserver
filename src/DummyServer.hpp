/*
Copyright (C) 2017 - G�bor "Razzie" G�rzs�ny
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
*/

#pragma once

#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <raz/networkbackend.hpp>
#include "Settings.hpp"

struct TCP {};
struct UDP {};

template<class>
class DummyServer;

template<>
class DummyServer<TCP>
{
public:
	static void run(uint16_t port, bool ipv6 = false)
	{
		raz::NetworkInitializer __init;
		DummyServer(port, ipv6).run();
	}

private:
	typedef raz::NetworkServerBackendTCP::Client Client;
	typedef raz::NetworkServerBackendTCP::ClientState ClientState;

	struct ClientComparator
	{
		bool operator()(const Client& a, const Client& b) const
		{
			return (a.socket < b.socket);
		}
	};

	DummyServer(uint16_t port, bool ipv6) :
		m_server_obj(port, ipv6),
		m_client_counter(0)
	{
	}

	void run()
	{
		Client client;
		ClientState state;
		char packet[MAX_PACKET_SIZE];

		for (;;)
		{
			size_t len = m_server_obj.wait(client, state, 1000);

			switch (state)
			{
			case ClientState::CLIENT_CONNECTED:
				handleClientConnected(client);
				break;

			case ClientState::CLIENT_DISCONNECTED:
				handleClientDisconnected(client);
				break;

			case ClientState::PACKET_RECEIVED:
				if (len > MAX_PACKET_SIZE)
				{
					std::cout << "too large packet, trimming size to " << MAX_PACKET_SIZE << " bytes" << std::endl;
					len = MAX_PACKET_SIZE;
				}
				m_server_obj.read(client, packet, len);
				handlePacket(packet, len, client);
				break;
			}
		}
	}

	void handleClientConnected(const Client& client)
	{
		m_client_map[client] = ++m_client_counter;
		std::cout << "#" << m_client_counter << " connected" << std::endl;
	}

	void handleClientDisconnected(const Client& client)
	{
		auto it = m_client_map.find(client);
		if (it != m_client_map.end())
		{
			std::cout << "#" << it->second << " disconnected" << std::endl;
			m_client_map.erase(it);
		}
	}

	void handlePacket(const char* packet, size_t len, Client& sender)
	{
		std::cout << "#" << m_client_map[sender] << " packet: " << len << " bytes" << std::flush;

		if (m_client_map.size() > 1)
			std::cout << " - sending to:";

		// sending packet to every other client
		for (auto& it : m_client_map)
		{
			if (it.first.socket != sender.socket)
			{
				std::cout << " #" << it.second << std::flush;
				m_server_obj.write(it.first, packet, len);
			}
		}

		std::cout << std::endl;
	}

	raz::NetworkServerBackendTCP m_server_obj;
	unsigned m_client_counter;
	std::map<Client, unsigned, ClientComparator> m_client_map;
};

template<>
class DummyServer<UDP>
{
public:
	static void run(uint16_t port, bool ipv6 = false)
	{
		raz::NetworkInitializer __init;
		DummyServer(port, ipv6).run();
	}

private:
	typedef raz::NetworkServerBackendUDP<MAX_PACKET_SIZE>::Client Client;
	typedef raz::NetworkServerBackendUDP<MAX_PACKET_SIZE>::ClientState ClientState;

	struct ClientComparator
	{
		bool operator()(const Client& a, const Client& b) const
		{
			return (std::memcmp(&a, &b, sizeof(Client)) < 0);
		}
	};

	DummyServer(uint16_t port, bool ipv6) :
		m_server_obj(port, ipv6),
		m_client_counter(0)
	{
	}

	void run()
	{
		Client client;
		ClientState state;
		char packet[MAX_PACKET_SIZE];

		for (;;)
		{
			size_t len = m_server_obj.wait(client, state, 1000);

			if (state == ClientState::PACKET_RECEIVED)
			{
				if (len > MAX_PACKET_SIZE)
				{
					std::cout << "too large packet, trimming size to " << MAX_PACKET_SIZE << " bytes" << std::endl;
					len = MAX_PACKET_SIZE;
				}
				m_server_obj.read(client, packet, len);
				handlePacket(packet, len, client);
				break;
			}
		}
	}

	void handlePacket(const char* packet, size_t len, Client& sender)
	{
		std::cout << "#" << m_client_map[sender] << " packet: " << len << " bytes" << std::flush;

		detectNewClient(sender);

		if (m_client_map.size() > 1)
			std::cout << " - sending to:";

		// sending packet to every other client
		for (auto& it : m_client_map)
		{
			if (it.first != sender)
			{
				std::cout << " #" << it.second << std::flush;

				try
				{
					m_server_obj.write(it.first, packet, len);
				}
				catch (raz::NetworkSocketError&)
				{
					handleClientDisconnect(it.first);
				}
			}
		}

		std::cout << std::endl;
	}

	void detectNewClient(const Client& client)
	{
		auto it = m_client_map.find(client);
		if (it == m_client_map.end())
		{
			m_client_map[client] = ++m_client_counter;
			std::cout << "#" << m_client_counter << " first packet (connected)" << std::endl;
		}
	}

	void handleClientDisconnect(const Client& client)
	{
		auto it = m_client_map.find(client);
		if (it != m_client_map.end())
		{
			std::cout << "#" << it->second << " not availably anymore (disconnected)" << std::endl;
			m_client_map.erase(it);
		}
	}

	raz::NetworkServerBackendUDP<MAX_PACKET_SIZE> m_server_obj;
	unsigned m_client_counter;
	std::map<Client, unsigned, ClientComparator> m_client_map;
};