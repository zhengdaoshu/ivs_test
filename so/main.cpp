/*
 * main.cpp
 *
 *  Created on: 2016Äê12ÔÂ27ÈÕ
 *      Author: J00054
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>       //setitimer
#include <time.h>           //struct timespec
#include "wt-calibration.h"
using namespace std;
#define CALIBRATION_FOLDER			"/calibration/"
string GetDir(void)
{
    char Buf[1024] = {0};
    int Cnt = readlink("/proc/self/exe", Buf, 1024);
    while (Cnt--)
    {
        if (Buf[Cnt] == '/')
        {
            Buf[Cnt] = '\0';
            break;
        }
    }
    return Buf;
}

static long long T1,T2;
void UTime_Start()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	T1 = tv.tv_sec*1000*1000+tv.tv_usec;
}
void UTime_End(int size)
{
	struct timeval tv;
	double rate;

	gettimeofday(&tv,NULL);
	T2 = tv.tv_sec*1000*1000+tv.tv_usec;
	printf("Time:%lldms, %lldus, rate: %4.3fMB/s\r\n", (T2-T1)/1000,(T2-T1), size*1.0/(T2-T1));
}

int main()
{
	int ret = 0;
	printf("start initial wt_calibration\n");
	char buf[10] = {0};
	char version[32] = {0};
	buf[0] = 'o';
	buf[1] = 'k';
	cout<<GetDir()<<endl;
	string dir = GetDir() + CALIBRATION_FOLDER + buf;
	cout<<"path="<<dir<<endl;
	UTime_Start();

	printf("wt_cal_so version:%s\n",wt_calibration_version());
	ret = wt_calibration_initial();
	UTime_End(1000);
	printf("initial ret=%d\n",ret);
	if(ret != 0)
	{
		printf("initial error,ret=%d\n",ret);
		exit(0);
	}
	char filename[32] ={0};
	sprintf(filename, "%s", "VSG/test.csv");
	char tmp_buf[100] = {0};
	int i = 0;
	for(i=0;i<100;i++)
	{
		tmp_buf[i] = i+1;
	}
	wt_calibration_set_calibration_data(filename, tmp_buf, 100);
	printf("filename=%s\n",filename);

	printf("version1=%s\n",wt_calibration_version());

	Ocxo_Parm ocxo;
	wt_calibration_get_ocxo_setting(&ocxo);
	printf("ocxo code=%d\n",ocxo.ocxo_code1);

	Rf_Config_Parm rf_config;
	ret = wt_calibration_get_configuration(&rf_config);
	printf("dac_margin=%lf\n",rf_config.dac_margin);

	Rx_Parm rx_parm;
	rx_parm.freq = 2412000000;
	rx_parm.unit = 0;
	rx_parm.ref_power = 40;
	rx_parm.rf_port = 6;
	rx_parm.temperature = 38.3;
	rx_parm.sw_temperature = 38.1;
	printf("rx_setting:freq=%lf\n",rx_parm.freq);
	ret = wt_calibration_get_rx_setting(&rx_parm, 240000000.0);
	//for(i=0;i<241;i++)
	{
		//printf("Rx_Parm bb_gain[%d]=%lf,rf_gain=%lf\n",i,rx_parm.rx_spec_flat_comp_parm.bb_comp_gain[i],rx_parm.rx_spec_flat_comp_parm.rf_comp_gain[i]);
	}
	printf("gain_imb=%lf,quad_err=%lf,timeskew=%lf\n",rx_parm.rx_iq_imb_parm.gain_imb,rx_parm.rx_iq_imb_parm.quad_err,rx_parm.rx_iq_imb_parm.timeskew);
	printf("i=%d,q=%d\n",rx_parm.rx_dc_offset_parm.i_code,rx_parm.rx_dc_offset_parm.q_code);
	printf("lo1_freq=%lf,lo2_freq=%lf\n",rx_parm.freq_parm.LoParm[0].freq,rx_parm.freq_parm.LoParm[1].freq);
	printf("actual_mpl=%lf,final_gain=%lf,att1=%d,att2=%d\n",rx_parm.rx_gain_parm.actual_mpl,rx_parm.rx_gain_parm.final_gain,rx_parm.rx_gain_parm.att_code[0],rx_parm.rx_gain_parm.att_code[1]);


	Tx_Parm tx_parm;
	tx_parm.freq = 2418000000;
	tx_parm.unit = 1;
	tx_parm.rf_port = 1;
	tx_parm.power = -30.0;
	tx_parm.temperature = 38.8;
	tx_parm.sw_temperature = 38.9;
	//tx_parm.tx_gain_parm.dac_gain = 0.0;
	printf("\n\n\n\ntx_setting:freq=%lf\n",tx_parm.freq);
	ret = wt_calibration_get_tx_setting(&tx_parm);
	for(i=0;i<241;i++)
	{
		//printf("bb_gain[%d]=%lf,rf_gain=%lf\n",i,tx_parm.tx_spec_flat_comp_parm.bb_comp_gain[i],tx_parm.tx_spec_flat_comp_parm.rf_comp_gain[i]);
	}
	printf("get_tx_setting ret=%d\n",ret);
	printf("lo1_freq=%lf,power=%d,lo2_freq=%lf,power=%d\n",tx_parm.freq_parm.LoParm[0].freq,tx_parm.freq_parm.LoParm[0].power_level,tx_parm.freq_parm.LoParm[1].freq,tx_parm.freq_parm.LoParm[1].power_level);
	printf("tx_parm,gain_imb=%lf,quad_err=%lf,timeskew=%lf\n",tx_parm.tx_iq_imb_parm.gain_imb,tx_parm.tx_iq_imb_parm.quad_err,tx_parm.tx_iq_imb_parm.timeskew);
	printf("i=%d,q=%d\n",tx_parm.tx_dc_offset_parm.i_code,tx_parm.tx_dc_offset_parm.q_code);
	printf("is_pa_on=%d,dac_gain=%lf,att1=%d,att2=%d,att3=%d,att4=%d\n",tx_parm.tx_gain_parm.is_pa_on,tx_parm.tx_gain_parm.dac_gain,tx_parm.tx_gain_parm.att_code[0],tx_parm.tx_gain_parm.att_code[1],tx_parm.tx_gain_parm.att_code[2],tx_parm.tx_gain_parm.att_code[3]);
	return 0;
}



