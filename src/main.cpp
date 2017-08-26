/*
Copyright (C) 2017 - Gábor "Razzie" Görzsöny
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

#include <cstdint>
#include <iostream>
#include <string>
#include "Settings.hpp"
#include "DummyServer.hpp"

enum class ServerType
{
	TCP,
	UDP
};

int main()
{
	ServerType server_type;
	uint16_t port;
	bool ipv6 = false;

	for (;;)
	{
		std::string input;

		std::cout << "Server type (tcp / udp): " << std::flush;
		std::cin >> input;

		if (input.compare("tcp") == 0)
		{
			server_type = ServerType::TCP;
			break;
		}
		else if (input.compare("udp") == 0)
		{
			server_type = ServerType::UDP;
			break;
		}
		else
		{
			continue;
		}
	}

	if (ASK_SERVER_IP_PROTOCOL)
	{
		for (;;)
		{
			std::string input;

			std::cout << "Server protocol (ipv4 / ipv6): " << std::flush;
			std::cin >> input;

			if (input.compare("ipv4") == 0)
			{
				ipv6 = false;
				break;
			}
			else if (input.compare("ipv6") == 0)
			{
				ipv6 = true;
				break;
			}
			else
			{
				continue;
			}
		}
	}

	std::cout << "Port: " << std::flush;
	std::cin >> port;

	try
	{
		if (server_type == ServerType::TCP)
			DummyServer<TCP>::run(port, ipv6);
		else
			DummyServer<UDP>::run(port, ipv6);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
