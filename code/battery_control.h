void init_ADC_battery_CH3(void);
void measure_ADC_battery_CH3(void);
void show_battery(void);
void battery_sleep_pdown(void);
void WDT_Disable(void);

#define SAMPLE_NUMBER_BT 16UL
#define dV ((V_REF*24UL)/1000UL)
#define BT_R1 470UL
#define BT_R2 150UL

uint16_t ADC_voltage_CH3;