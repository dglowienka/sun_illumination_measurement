#include <stdbool.h>

#define SAMPLE_NUMBER 512UL
#define V_REF (32700UL/16UL)
#define TOP 4095UL

void start_measurements(void);
void freq_measurements(void);
void single_measurements(void);
void multi_measurements(void);
void duration_measurements(void);
bool init_start_measurements(void);

void init_ADCA(void);
void init_ADC_CH1(void);
uint16_t measure_ADC_CH1(void);
void init_ADC_CH2(void);
uint16_t measure_ADC_CH2(void);

extern const unsigned char MN19[];
extern const unsigned char MN20[];
extern const unsigned char MN21[];
extern const unsigned char MN22[];
extern const unsigned char MN23[];

extern uint32_t measure_time_seconds;
extern uint16_t measure_frequency_seconds;

extern struct calendar_date time_start_measurement;
bool start_measurements_global;
uint16_t irradiation_CH2;