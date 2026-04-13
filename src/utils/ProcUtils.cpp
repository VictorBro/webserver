#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iostream>
#include "utils/ProcUtils.hpp"
#include "utils/Globals.hpp"

pid_t doWaitpid(pid_t pid, int options, int &status, int &code)
{
	int err;
	pid_t exitedPid = waitpid(pid, &status, options);
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
	if (WIFEXITED(status))
	{
		if (DEBUG)
			std::cout << exitedPid << ": child process exited with status: " << WEXITSTATUS(status) << std::endl;
		code = WEXITSTATUS(status);
	}
	else if (WIFSIGNALED(status))
	{
		if (DEBUG)
			std::cout << exitedPid << ": child process terminated by signal: " << 128 + WTERMSIG(status) << std::endl;
		code = WTERMSIG(status) + 128;
	}
	else
	{
		std::cerr << exitedPid << ": child process terminated abnormally" << std::endl;
		code = -1;
	}
	return (exitedPid);
}
