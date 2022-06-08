#ifndef __UTILS_H
#define __UTILS_H

#include <iostream>
#include <string>
#include <sys/syscall.h>
#include <sys/types.h>
#include <cxxabi.h>
#include <unistd.h>

namespace mysylar {

inline static pid_t GetThreadId() { return syscall(SYS_gettid); }
inline static uint32_t GetFiberId() { return 0; }
inline static std::string GetThreadName() { return "name"; }

template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}
}

#endif