/*
 * @Author       : 陈科进
 * @Date         : 2023-05-21 12:14:37
 * @LastEditors  : 陈科进
 * @LastEditTime : 2023-05-21 12:18:36
 * @FilePath     : \main\usb_cherry\CherryUSB\class\usbcan\usb_util.h
 * @Description  : ESP32 candlelight firmware object
 */
#ifndef __USB_HELP_H__
#define __USB_HELP_H__

#define TU_U16_HIGH(u16)   ((uint8_t)(((u16) >> 8) & 0x00ff))
#define TU_U16_LOW(u16)    ((uint8_t)((u16)&0x00ff))
#define U16_TO_U8S_LE(u16) TU_U16_LOW(u16), TU_U16_HIGH(u16)

#define TU_STRING(x)     #x              ///< stringify without expand
#define TU_XSTRING(x)    TU_STRING(x)    ///< expand then stringify
#define TU_STRCAT(a, b)  a##b            ///< concat without expand
#define TU_XSTRCAT(a, b) TU_STRCAT(a, b) ///< expand then concat

// Compile-time Assert
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define TU_VERIFY_STATIC _Static_assert
#elif defined(__cplusplus) && __cplusplus >= 201103L
#define TU_VERIFY_STATIC static_assert
#else
#define TU_VERIFY_STATIC(const_expr, _mess) enum { TU_XSTRCAT(_verify_static_, _TU_COUNTER_) = 1 / (!!(const_expr)) }
#endif

#endif