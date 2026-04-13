#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iostream>
#include "utils/ProcUtils.hpp"
#include "utils/Globals.hpp"

pid_t doWaitpid(pid_t pid, int options, int &statusOut, int &codeOut)
{
	int err;
	pid_t exitedPid = waitpid(pid, &statusOut, options);
	err = errno;
	if (exitedPid == -1)
	{
		if (err == ECHILD)
		{
			// if (DEBUG)
				// std::cout << "waitpid: No child processes" << std::endl;
		}
		else if (err == EINTR)
		{
			if (DEBUG)
				std::cout << "waitpid: Interrupted by signal" << std::endl;
		}
		else
		{
			std::cerr << "waitpid: waitpid error: " << strerror(err) << std::endl;
		}
		return -1;
	}
	if (exitedPid == 0)
	{
		// No status available (WNOHANG was specified and no child has exited)
		return 0;
	}
	if (WIFEXITED(statusOut))
	{
		if (DEBUG)
			std::cout << exitedPid << ": child process exited with status: " << WEXITSTATUS(statusOut) << std::endl;
		codeOut = WEXITSTATUS(statusOut);
	}
	else if (WIFSIGNALED(statusOut))
	{
		if (DEBUG)
			std::cout << exitedPid << ": child process terminated by signal: " << 128 + WTERMSIG(statusOut) << std::endl;
		codeOut = WTERMSIG(statusOut) + 128;
	}
	else
	{
		std::cerr << exitedPid << ": child process terminated abnormally" << std::endl;
		codeOut = -1;
	}
	return (exitedPid);
}
