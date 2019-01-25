#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

// Flag for debug
// #define __DEBUG

// Flag for memcheck
// #define __MEM_CHECK

#if defined(__linux__)
    #ifdef __ANDROID__
        #define __OS_Android__
    #else
        #define __OS_Linux__
    #endif
#elif defined(_WIN32)
    #define __OS_Windows__
#endif

// Other platforms...

// Flag for compile samples
#if defined(__OS_Linux__) || defined(__OS_Windows__)
    #define __USE_SAMPLES
#endif

#endif  // CONFIG_H_INCLUDED
