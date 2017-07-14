#ifndef UTILS_UTILSPRINT_H_
#define UTILS_UTILSPRINT_H_

#ifdef DEBUG_SERIAL
    #include <mbed.h>
    extern Serial pc;
    #define PRINTF_INITIALIZE()
    #define PRINTF(FORMAT,args...) pc.printf(FORMAT, ##args)
#else
    #define PRINTF_INITIALIZE()
    #define PRINTF(FORMAT,args...)
#endif

#endif // UTILS_UTILSPRINT_H_
