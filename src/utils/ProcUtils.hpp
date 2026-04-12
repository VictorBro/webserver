#pragma once
#include <sys/types.h>
#include <sys/wait.h>

pid_t doWaitpid(pid_t pid, int options, int* statusOut, int *code);
