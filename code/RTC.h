char time_RTC[10];
char date_RTC[10];

volatile uint32_t seconds;

void timer_RTC(void);
void show_RTC(void);
void RTC_time(void);
void RTC_date(void);