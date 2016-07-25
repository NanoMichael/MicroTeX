#define MEM_CHECK

/**
 * !IMPORTANT undefine MEM_CHECK before release
 */
#undef MEM_CHECK

#ifndef MEM_CHECK
#define DUMP_MEM_STATUS() ;
#define MEMCHECK_H_INCLUDED
#endif // MEM_CHECK

#ifndef MEMCHECK_H_INCLUDED
#define MEMCHECK_H_INCLUDED

#include <cstddef>  // for size_t

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

#endif // MEMCHECK_H_INCLUDED
