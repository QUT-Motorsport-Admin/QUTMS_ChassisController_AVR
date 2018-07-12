#include <stdlib.h>
#include <avr/io.h>
#include "utils/a2d.h"

#define ADC_SAMPLES	(10) /**< Samples required for ADC */

uint16_t adc_read_avg(uint8_t ch);