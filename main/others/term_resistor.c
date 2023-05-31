/*
 * @Author       : 陈科进
 * @Date         : 2023-05-13 18:47:54
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-13 19:01:14
 * @FilePath     : \main\others\term_resistor.c
 * @Description  : ESP32 candlelight firmware object
 */
#include "term_resistor.h"
inline enum TerminatorStatus setTermResistor(unsigned int channel, enum TerminatorStatus state)
{
    (void)channel;
    (void)state;
    return TERM_UNSUPPORTED;
}

inline enum TerminatorStatus getTermResistor(unsigned int channel)
{
    (void)channel;
    return TERM_UNSUPPORTED;
}
