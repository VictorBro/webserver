#include <iostream>
#include <csignal>
#include <cstdio>
#include "utils/Consts.hpp"
#include "utils/Globals.hpp"
#include "server/HttpServer.hpp"

bool g_running = true;

void sigintHandler(int)
{
	g_running = false;
}

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	// Signal handling setup
	struct sigaction sa;
	sa.sa_handler = sigintHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return 1;
	}

	try
	{
		std::string filename = (argc == 2) ? argv[1] : kDefaultConfig;
		HttpServer ws(filename);
		if (DEBUG)
			ws.printSettings();

		ws.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
