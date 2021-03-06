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

#include <cstdint>
#include <map>
#include <raz/networkbackend.hpp>
#include "Settings.hpp"

class DummyServerTCP
{
public:
	static void run(uint16_t port, bool ipv6 = false);

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

	DummyServerTCP(uint16_t port, bool ipv6);
	void run();
	void handleClientConnect(const Client& client);
	void handleClientDisconnect(const Client& client);
	void handlePacket(const char* packet, size_t len, Client& sender);

	raz::NetworkServerBackendTCP m_server_obj;
	unsigned m_client_counter;
	std::map<Client, unsigned, ClientComparator> m_client_map;
};
