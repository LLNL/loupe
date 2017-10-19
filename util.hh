#ifndef __UTIL_HH
#define __UTIL_HH

#include <map>

extern std::map<uint64_t, std::string> g_symbols;

void backtrace(uint64_t *pc_val);

#endif
