#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

// Flag for debug
#if defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG__) || !defined(NDEBUG)
#   ifndef __DEBUG
#       define __DEBUG
#   endif
#endif

// Check platforms
#if defined(__linux__)
#   ifdef __ANDROID__
#       define __OS_Android__
#   else
#       define __OS_Linux__
#   endif
#elif defined(__APPLE__) && defined(__MACH__)
#   define __OS_Mac__
#elif defined(_WIN32)
#   define __OS_Windows__
#endif
// Other platforms...

#if defined(__OS_Linux__) || defined(__OS_Mac__)
#   define __OS_Unix_like_PC__
#endif

// Flag for if compile samples
#if defined(__OS_Unix_like_PC__) || defined(__OS_Windows__) || defined(MEM_CHECK)
#   define __USE_SAMPLES
#endif

// Disable log if not in debug mode
#ifndef __DEBUG
#   undef HAVE_LOG
#endif

#endif  // CONFIG_H_INCLUDED
