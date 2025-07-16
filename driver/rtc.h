#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint16_t year;  // 1970-2099
  uint8_t month;  // 1-12
  uint8_t day;    // 1-31
  uint8_t hour;   // 0-23
  uint8_t minute; // 0-59
  uint8_t second; // 0-59
} rtc_date_t;

void rtc_init(void);
void rtc_set_date(rtc_date_t *date);
void rtc_get_date(rtc_date_t *date);
void rtc_set_timestamp(uint32_t timestamp);
void rtc_get_timestamp(uint32_t *timestamp);

#endif // RTC_H