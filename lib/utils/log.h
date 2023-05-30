#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef HAVE_LOG

#include <cstdio>

// clang-format off
#define ANSI_BOLD        "\x1b[1m"
#define ANSI_COLOR_CYAN  "\x1b[36m"
#define ANSI_COLOR_RED   "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_UNDERLINE   "\x1b[4m"
#define ANSI_RESET       "\x1b[0m"
// clang-format on

#define dbg(format, ...)                                                              \
  fprintf(                                                                            \
    stdout,                                                                           \
    "FILE: " ANSI_UNDERLINE "%s" ANSI_RESET ", LINE: " ANSI_COLOR_RED "%d" ANSI_RESET \
    ", FUNCTION: " ANSI_COLOR_CYAN "%s" ANSI_RESET ", MSG: " format,                  \
    __FILE__,                                                                         \
    __LINE__,                                                                         \
    __FUNCTION__,                                                                     \
    ##__VA_ARGS__                                                                     \
  );

#define logv(format, ...) fprintf(stdout, format, ##__VA_ARGS__);

#define loge(format, ...) fprintf(stderr, format, ##__VA_ARGS__);

#endif  // HAVE_LOG

#endif
