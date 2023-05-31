/*
 * @Author       : 陈科进
 * @Date         : 2023-05-13 18:48:16
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-13 18:51:46
 * @FilePath     : \main\additions\term_resistor.h
 * @Description  : ESP32 candlelight firmware object
 */

#ifndef __TERM_RESISTOR_H__
#define __TERM_RESISTOR_H__
enum TerminatorStatus // 开启120欧姆终端电阻
{
    TERM_UNSUPPORTED = -1,
    TERM_INACTIVE,
    TERM_ACTIVE
};

enum TerminatorStatus setTermResistor(unsigned int channel, enum TerminatorStatus state);
enum TerminatorStatus getTermResistor(unsigned int channel);

#endif