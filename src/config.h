#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

// Flag for debug
// #define __DEBUG

// Flag for graphic debug
#define __GA_DEBUG

// Flag for memcheck
// #define __MEM_CHECK

// Flag for compile samples
#ifndef __ANDROID__
    #define __USE_SAMPLES
#endif // __ANDROID__

#endif // CONFIG_H_INCLUDED
