#include "LMT70.h"

extern ADC_HandleTypeDef hadc1;

uint16_t ADC_ConvertedValue[20];
double Temperature;

float Get_LMT70_Temperature(void)
{
		unsigned int i = 0, ADC_Value = 0;
		double Temperature;
		double Tempcx;
		for(i = 0; i < 20; ++i)
		{
			ADC_Value += ADC_ConvertedValue[i];
		}
		ADC_Value = ADC_Value / 20;
		Tempcx = (double)ADC_Value*(3.3/4096)*1000;
		Temperature = (-0.0000000010642)*Tempcx*Tempcx*Tempcx+(-0.000005759725)*Tempcx*Tempcx+(-0.1789883)*Tempcx+204.857;		
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_ConvertedValue[0], 20);
		
		return Temperature;
}

