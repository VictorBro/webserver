#pragma once
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Wrapper around waitpid(). Waits for a child process and extracts
 * the exit status or signal information.
 *
 * @param pid       PID to wait for (-1 for any child).
 * @param options   waitpid options (e.g. WNOHANG).
 * @param statusOut Raw status value filled by waitpid().
 * @param codeOut   Decoded exit code (WEXITSTATUS), signal number + 128
 *                  (WTERMSIG), or -1 for abnormal termination.
 * @return          The PID of the exited child, 0 if WNOHANG and no child
 *                  has exited, or -1 on error.
 */
pid_t doWaitpid(pid_t pid, int options, int &statusOut, int &codeOut);
