#ifndef MEMCHECK_H_INCLUDED
#define MEMCHECK_H_INCLUDED

#include "config.h"

#ifndef __MEM_CHECK
#define DUMP_MEM_STATUS() ;
#define MEM_ON() ;
#define MEM_OFF() ;
#define TRACE_ON() ;
#define TRACE_OFF() ;
#else

#include <cstddef>

// Hijack the new operator (both scalar and array versions)
void* operator new(std::size_t, const char*, long);
void* operator new[](std::size_t, const char*, long);
#define new new (__FILE__, __LINE__)

extern bool traceFlag;
#define TRACE_ON() traceFlag = true
#define TRACE_OFF() traceFlag = false

extern bool activeFlag;
#define MEM_ON() activeFlag = true
#define MEM_OFF() activeFlag = false

void print_mem(const char*, long);
#define DUMP_MEM_STATUS() print_mem(__FILE__, __LINE__)

#endif // __MEM_CHECK
#endif // MEMCHECK_H_INCLUDED
