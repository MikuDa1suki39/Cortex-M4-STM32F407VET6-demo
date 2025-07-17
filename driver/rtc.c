#include <string.h>
#include <stdbool.h>

#include "stm32f4xx.h"

#include "rtc.h"

// 日期验证函数（与之前相同）
static bool date_validate(const rtc_date_t *date)
{
    if (date->year < 1970 || date->year > 2099)
        return false;
    if (date->month < 1 || date->month > 12)
        return false;
    if (date->day < 1 || date->day > 31)
        return false;
    if (date->hour > 23)
        return false;
    if (date->minute > 59)
        return false;
    if (date->second > 59)
        return false;
    return true;
}

// 日期转时间戳（与之前相同）
static uint32_t date_to_ts(const rtc_date_t *date)
{
    uint16_t year = date->year;
    uint8_t month = date->month;
    uint8_t day = date->day;
    uint8_t hour = date->hour;
    uint8_t minute = date->minute;
    uint8_t second = date->second;

    // 调整月份计算
    month -= 2;
    if (month <= 0)
    {
        month += 12;
        year -= 1;
    }

    // 计算时间戳
    return (((year / 4 - year / 100 + year / 400 + 367 * month / 12 + day +
              year * 365 - 719499) *
                 24 +
             hour) *
                60 +
            minute) *
               60 +
           second;
}

// 时间戳转日期（与之前相同）
static void ts_to_date(uint32_t seconds, rtc_date_t *date)
{
    const uint32_t mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const uint16_t ONE_YEAR_HOURS = 8760;

    memset(date, 0, sizeof(rtc_date_t));

    // 计算秒/分
    date->second = seconds % 60;
    seconds /= 60;
    date->minute = seconds % 60;
    seconds /= 60;

    // 计算年
    uint32_t leapyears = seconds / (1461 * 24);
    date->year = (leapyears << 2) + 1970;
    seconds %= 1461 * 24;

    // 计算剩余时间
    while (1)
    {
        uint32_t yearhours = ONE_YEAR_HOURS;
        if ((date->year & 3) == 0)
            yearhours += 24; // 闰年

        if (seconds < yearhours)
            break;

        date->year++;
        seconds -= yearhours;
    }

    // 计算时
    date->hour = seconds % 24;
    seconds /= 24;
    seconds++;

    // 处理闰年2月29日
    if ((date->year & 3) == 0)
    {
        if (seconds == 60)
        {
            date->month = 2;
            date->day = 29;
            return;
        }
        else if (seconds > 60)
        {
            seconds--;
        }
    }

    // 计算月/日
    date->month = 0;
    while (seconds > mdays[date->month])
    {
        seconds -= mdays[date->month];
        date->month++;
    }
    date->month++;
    date->day = seconds;
}

// RTC初始化
void rtc_init(void)
{
    // 配置RTC预分频器，得到1Hz的时钟
    RTC_InitTypeDef RTC_InitStruct;
    RTC_InitStruct.RTC_AsynchPrediv = 255; // 异步分频值：255，则异步时钟频率=32768/(255+1)=256Hz
    RTC_InitStruct.RTC_SynchPrediv = 127;  // 同步分频值：127，则同步时钟频率=128/(127+1)=1Hz
    RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;

    // 初始化RTC
    RTC_Init(&RTC_InitStruct);

    // 选择LSE作为RTC时钟源
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    // 等待RTC寄存器同步
    RTC_WaitForSynchro();
}

// 设置日期时间
void rtc_set_date(rtc_date_t *date)
{
    if (!date_validate(date))
        return;

    // 创建标准库的时间结构体
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_TimeStruct.RTC_H12 = RTC_H12_AM; // 24小时制
    RTC_TimeStruct.RTC_Hours = date->hour;
    RTC_TimeStruct.RTC_Minutes = date->minute;
    RTC_TimeStruct.RTC_Seconds = date->second;

    // 创建标准库的日期结构体
    RTC_DateTypeDef RTC_DateStruct;
    RTC_DateStruct.RTC_Year = date->year - 1970; // 将年份转换为相对值
    RTC_DateStruct.RTC_Month = date->month;
    RTC_DateStruct.RTC_Date = date->day;

    // 设置时间和日期（使用二进制格式）
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

// 获取日期时间
void rtc_get_date(rtc_date_t *date)
{
    if (!date)
        return;

    // 创建标准库的时间结构体
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

    // 创建标准库的日期结构体
    RTC_DateTypeDef RTC_DateStruct;
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

    // 将标准库结构体转换为我们自定义的结构体
    date->hour = RTC_TimeStruct.RTC_Hours;
    date->minute = RTC_TimeStruct.RTC_Minutes;
    date->second = RTC_TimeStruct.RTC_Seconds;
    date->year = RTC_DateStruct.RTC_Year + 1960; // 恢复年份
    date->month = RTC_DateStruct.RTC_Month;
    date->day = RTC_DateStruct.RTC_Date;
}

// 设置时间戳
void rtc_set_timestamp(uint32_t timestamp)
{
    rtc_date_t date;
    ts_to_date(timestamp, &date);
    rtc_set_date(&date);
}

// 获取时间戳
void rtc_get_timestamp(uint32_t *timestamp)
{
    if (!timestamp)
        return;

    rtc_date_t date;
    rtc_get_date(&date);
    *timestamp = date_to_ts(&date);
}

// #include "stm32f4xx.h"
// #include "rtc.h"
// #include <string.h>
// #include <stdbool.h>

// // 日期验证函数
// static bool date_validate(const rtc_date_t *date)
// {
//     if (date->year < 1970 || date->year > 2099)
//         return false;
//     if (date->month < 1 || date->month > 12)
//         return false;
//     if (date->day < 1 || date->day > 31)
//         return false;
//     if (date->hour > 23)
//         return false;
//     if (date->minute > 59)
//         return false;
//     if (date->second > 59)
//         return false;
//     return true;
// }

// // 日期转时间戳
// uint32_t date_to_ts(const rtc_date_t *date)
// {
//     uint16_t year = date->year;
//     uint8_t month = date->month;
//     uint8_t day = date->day;
//     uint8_t hour = date->hour;
//     uint8_t minute = date->minute;
//     uint8_t second = date->second;

//     // 调整月份计算
//     month -= 2;
//     if (month <= 0)
//     {
//         month += 12;
//         year -= 1;
//     }

//     // 计算时间戳
//     return (((year / 4 - year / 100 + year / 400 + 367 * month / 12 + day +
//               year * 365 - 719499) *
//                  24 +
//              hour) *
//                 60 +
//             minute) *
//                60 +
//            second;
// }

// // 时间戳转日期
// void ts_to_date(uint32_t seconds, rtc_date_t *date)
// {
//     const uint32_t mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//     const uint16_t ONE_YEAR_HOURS = 8760;

//     memset(date, 0, sizeof(rtc_date_t));

//     // 计算秒/分
//     date->second = seconds % 60;
//     seconds /= 60;
//     date->minute = seconds % 60;
//     seconds /= 60;

//     // 计算年
//     uint32_t leapyears = seconds / (1461 * 24);
//     date->year = (leapyears << 2) + 1970;
//     seconds %= 1461 * 24;

//     // 计算剩余时间
//     while (1)
//     {
//         uint32_t yearhours = ONE_YEAR_HOURS;
//         if ((date->year & 3) == 0)
//             yearhours += 24; // 闰年

//         if (seconds < yearhours)
//             break;

//         date->year++;
//         seconds -= yearhours;
//     }

//     // 计算时
//     date->hour = seconds % 24;
//     seconds /= 24;
//     seconds++;

//     // 处理闰年2月29日
//     if ((date->year & 3) == 0)
//     {
//         if (seconds == 60)
//         {
//             date->month = 2;
//             date->day = 29;
//             return;
//         }
//         else if (seconds > 60)
//         {
//             seconds--;
//         }
//     }

//     // 计算月/日
//     date->month = 0;
//     while (seconds > mdays[date->month])
//     {
//         seconds -= mdays[date->month];
//         date->month++;
//     }
//     date->month++;
//     date->day = seconds;
// }

// // RTC初始化
// void rtc_init(void)
// {
//     // 启用PWR和备份域时钟
//     RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//     PWR->CR |= PWR_CR_DBP; // 启用备份域访问

//     // 配置LSE时钟
//     RCC->BDCR |= RCC_BDCR_LSEON;
//     while (!(RCC->BDCR & RCC_BDCR_LSERDY))
//         ; // 等待LSE就绪

//     // 选择LSE作为RTC时钟源
//     RCC->BDCR &= ~RCC_BDCR_RTCSEL;
//     RCC->BDCR |= RCC_BDCR_RTCSEL_0; // LSE
//     RCC->BDCR |= RCC_BDCR_RTCEN;    // 启用RTC时钟

//     // 配置预分频器 (LSE = 32.768 kHz)
//     RTC->WPR = 0xCA; // 禁用写保护
//     RTC->WPR = 0x53;
//     RTC->ISR |= RTC_ISR_INIT; // 进入初始化模式
//     while (!(RTC->ISR & RTC_ISR_INITF))
//         ; // 等待初始化完成

//     // 设置预分频器 (32768/32767 = 1Hz)
//     RTC->PRER = (127 << 16) | (255); // 异步127, 同步255
//     RTC->ISR &= ~RTC_ISR_INIT;       // 退出初始化模式
//     RTC->WPR = 0xFF;                 // 启用写保护
// }

// // 设置日期
// void rtc_set_date(rtc_date_t *date)
// {
//     if (!date_validate(date))
//         return;

//     RTC->WPR = 0xCA; // 禁用写保护
//     RTC->WPR = 0x53;
//     RTC->ISR |= RTC_ISR_INIT; // 进入初始化模式
//     while (!(RTC->ISR & RTC_ISR_INITF))
//         ;

//     // 设置时间（二进制转BCD）
//     RTC->TR = ((date->hour / 10) << 20) | ((date->hour % 10) << 16) |
//               ((date->minute / 10) << 12) | ((date->minute % 10) << 8) |
//               ((date->second / 10) << 4) | (date->second % 10);

//     // 设置日期（二进制转BCD）
//     RTC->DR = ((date->year / 10) << 20) | ((date->year % 10) << 16) |
//               ((date->month / 10) << 12) | ((date->month % 10) << 8) |
//               ((date->day / 10) << 4) | (date->day % 10);

//     RTC->ISR &= ~RTC_ISR_INIT; // 退出初始化模式
//     RTC->WPR = 0xFF;           // 启用写保护
// }

// // 获取日期
// void rtc_get_date(rtc_date_t *date)
// {
//     if (!date)
//         return;

//     uint32_t tr = RTC->TR;
//     uint32_t dr = RTC->DR;

//     // 时间BCD转二进制
//     date->hour = ((tr >> 16) & 0x3F) + 10 * ((tr >> 20) & 0x3);
//     date->minute = ((tr >> 8) & 0x7F) + 10 * ((tr >> 12) & 0x7);
//     date->second = (tr & 0x7F) + 10 * ((tr >> 4) & 0x7);

//     // 日期BCD转二进制
//     date->year = ((dr >> 16) & 0xFF) + 10 * ((dr >> 20) & 0xF);
//     date->month = ((dr >> 8) & 0x1F) + 10 * ((dr >> 12) & 0x1);
//     date->day = (dr & 0x3F) + 10 * ((dr >> 4) & 0x3);
// }

// // 设置时间戳
// void rtc_set_timestamp(uint32_t timestamp)
// {
//     rtc_date_t date;
//     ts_to_date(timestamp, &date);
//     rtc_set_date(&date);
// }

// // 获取时间戳
// void rtc_get_timestamp(uint32_t *timestamp)
// {
//     if (!timestamp)
//         return;

//     rtc_date_t date;
//     rtc_get_date(&date);
//     *timestamp = date_to_ts(&date);
// }