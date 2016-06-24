#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
using namespace std;

#include "memcheck.h"

#ifdef MEM_CHECK
#undef new

#define __mem_log printf

// Global flags set by macros in memcheck.h
bool traceFlag = false;
bool activeFlag = true;

namespace {

// Memory map entry type
struct Info {
	void* ptr;
	size_t sz;
	const char* file;
	long line;
};

// Memory map data
const size_t MAXPTRS = 30000u;
Info memMap[MAXPTRS];
size_t nptrs = 0;

// Searches the map for an address
int findPtr(void* p) {
	for (size_t i = 0; i < nptrs; ++i)
		if (memMap[i].ptr == p)
			return i;
	return -1;
}

void delPtr(void* p) {
	int pos = findPtr(p);
	assert(p >= 0);
	// Remove pointer from map
	for (size_t i = pos; i < nptrs-1; ++i)
		memMap[i] = memMap[i+1];
	--nptrs;
}

const int OUT_FILE_LEN = 40;

// Dummy type for static destructor
struct Sentinel {
	~Sentinel() {
		if (nptrs > 0) {
			__mem_log("Leaked memory at:\n");
			__mem_log("\t----------------------------------------------------------------------\n");
			__mem_log("\t| Address  |   Size  |%22s%18s| Line |\n", "File", " ");
			__mem_log("\t----------------------------------------------------------------------\n");
			for (size_t i = 0; i < nptrs; ++i) {
				int space = OUT_FILE_LEN / 2 + strlen(memMap[i].file) / 2;
				int rest = OUT_FILE_LEN - space;
				__mem_log("\t| %p | %6d  |%*s%*s| %-4ld | \n", memMap[i].ptr, memMap[i].sz, space, memMap[i].file, rest, " ", memMap[i].line);
				__mem_log("\t----------------------------------------------------------------------\n");
			}
		} else
			__mem_log("No user memory leaks!\n");
	}
};

// Static dummy object
Sentinel s;

} // End anonymous namespace

void print_mem(const char* file, long line) {
	size_t sz = 0;
	for (size_t i = 0; i < nptrs; i++)
		sz += memMap[i].sz;
	__mem_log("Current memory status:\n");
	__mem_log("\t---------------------------------------------------------------------\n");
	__mem_log("\t| objects |   size  |%22s%18s| Line |\n", "File", " ");
	__mem_log("\t---------------------------------------------------------------------\n");
	int space = OUT_FILE_LEN / 2 + strlen(file) / 2;
	int rest = OUT_FILE_LEN - space;
	__mem_log("\t|%8u |%8u |%*s%*s| %-4ld |\n", nptrs, sz, space, file, rest, " ", line);
	__mem_log("\t---------------------------------------------------------------------\n");
}

// Overload scalar new
void* operator new(size_t siz, const char* file, long line) {
	void* p = malloc(siz);
	if (activeFlag) {
		if (nptrs == MAXPTRS) {
			__mem_log("memory map too small (increase MAXPTRS)\n");
			exit(1);
		}
		memMap[nptrs].ptr = p;
		memMap[nptrs].sz = siz;
		memMap[nptrs].file = file;
		memMap[nptrs].line = line;
		++nptrs;
	}
	if (traceFlag) {
		__mem_log("Allocated %u bytes at address %p ", siz, p);
		__mem_log("(file: %s, line: %ld)\n", file, line);
	}
	return p;
}

// Overload array new
void* operator new[](size_t siz, const char* file, long line) {
	return operator new(siz, file, line);
}

// Override scalar delete
void operator delete(void* p) {
	if (findPtr(p) >= 0) {
		free(p);
		assert(nptrs > 0);
		delPtr(p);
		if (traceFlag)
			__mem_log("Deleted memory at address %p\n", p);
	} else if (!p && activeFlag)
		__mem_log("Attempt to delete unknown pointer: %p\n", p);
}

// Override array delete
void operator delete[](void* p) {
	operator delete(p);
}

#endif
