#ifndef __UTILS_H
#define __UTILS_H

#include <iostream>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>

namespace mysylar {

inline static pid_t GetThreadId() { return syscall(SYS_gettid); }
inline static uint32_t GetFiberId() { return 0; }
inline static std::string GetThreadName() { return "name"; }
}

#endif