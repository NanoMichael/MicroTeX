#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include "config.h"

#include <cstdio>

#define ANSI_BOLD "\x1b[1m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_RESET "\x1b[0m"

#define __dbg(format, ...)             \
  {                                    \
    fprintf(                           \
        stdout,                        \
        "FILE: " ANSI_UNDERLINE        \
        "%s" ANSI_RESET                \
        ", LINE: " ANSI_COLOR_RED      \
        "%d" ANSI_RESET                \
        ", FUNCTION: " ANSI_COLOR_CYAN \
        "%s" ANSI_RESET                \
        ", MSG: " format,              \
        __FILE__,                      \
        __LINE__,                      \
        __FUNCTION__,                  \
        ##__VA_ARGS__);                \
  }

#ifdef HAVE_LOG
#include <iostream>
#define __log std::cout
#endif

#define __print printf

#endif
