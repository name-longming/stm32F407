void adc_filter(ADC_struct_m *p)
{
    int16 i = 0, j = 0;
    uint16 AD_sum[7] = { 0,0,0,0,0,0,0};
    static uint16 AD_valu[7][3];

    /*****读取七电感平均值*****/
    p->adc_primary_value[0] = adc_mean_filter(ADC_0, myADC1_ch0, ADC_12BIT, 5);//最左
    p->adc_primary_value[1] = adc_mean_filter(ADC_0, myADC1_ch1, ADC_12BIT, 5);
    p->adc_primary_value[2] = adc_mean_filter(ADC_0, myADC1_ch2, ADC_12BIT, 5);
    p->adc_primary_value[3] = adc_mean_filter(ADC_0, myADC1_ch4, ADC_12BIT, 5);//Midle
    p->adc_primary_value[4] = adc_mean_filter(ADC_0, myADC1_ch5, ADC_12BIT, 5);
    p->adc_primary_value[5] = adc_mean_filter(ADC_0, myADC1_ch6, ADC_12BIT, 5);
    p->adc_primary_value[6] = adc_mean_filter(ADC_0, myADC1_ch7, ADC_12BIT, 5);//最右

    /*****************滑动滤波，采用三次平均值数据进行滤波******************/
    for(j=0;j<7;j++) //滑动滤波
    {
        AD_valu[j][0] = AD_valu[j][1];      //模拟FiFo移除第一次采集到的数据
        AD_valu[j][1] = AD_valu[j][2];      //模拟FiFo移除第一次采集到的数据
    }
    for(i=0;i<7;i++)
    {
        AD_valu[i][2] = p->adc_primary_value[i];    //补充最新一次采集到的数据
    }

    for(j=0;j<7;j++)                               //三次数据进行求和
    {
        for(i=0;i<3;i++)
        {
            AD_sum[j] += AD_valu[j][i];
        }
    }
    for(i=0;i<7;i++)                    //求均值，并进行归一化处理
    {
        p->adc_filter_value[i] = AD_sum[i]/3; //求均值
        p->adc_filter_value[i] = (int)(p->adc_filter_value[i]*400*1.0/
        (p->adc_max_value - p->adc_min_value));//归一化（0-400）
    }
}