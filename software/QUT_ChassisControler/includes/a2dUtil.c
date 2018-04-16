/**
 * adc_read_avg()
 * Input:	ch	-	The analog input channel
 * Returns: 10 bit digital value representative of the analog input in the specified channel
 * 
 * The inbuilt ADC is used to convert an analog input voltage at the specified channel to a 10-bit digital 
 * value through successive approximation.
 * This is done <ADC_SAMPLES> times and the results are averaged in order to obtain a more accurate value.
 * 
 * Reference: ATmega Datasheet Chapter 26 (ADC - Analog to Digital Converter)
 **/
uint16_t adc_read_avg(uint8_t ch)
{
	uint32_t adcSUM = 0;
	for(uint8_t i = 0; i<ADC_SAMPLES; i++)
	{
		adcSUM += a2d_10bitCh(ch);
	}
	return (uint16_t)(adcSUM/ADC_SAMPLES);
}