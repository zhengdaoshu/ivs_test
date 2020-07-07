/*
 * conf.h
 *
 *  Created on: 2016��12��13��
 *      Author: J00054
 */
#include "../csv_file_manager/csv_disk.h"
#include "../wt-calibration.h"
#include "lo_gain_config.h"
#ifndef INCLUDE_CONFIGURATION_CONFIG_H_
#define INCLUDE_CONFIGURATION_CONFIG_H_

typedef struct RF_Configuration_Data
{
	double adc_expected_power;
	double bbvga_expected_gain;
	double dac_margin;
	double dac_margin_ax;
	double tx_pa_threshold_5g;
	double tx_pa_threshold_2g;
	int tx_temp_cps;                /* tx�¶Ȳ������� */
	int rx_temp_cps;
	int tx_subband1;				/* 5GС�ڸ�Ƶ����õ�һ��Ƶ */
	int tx_subband2;                /* 5GС�ڸ�Ƶ����õڶ���Ƶ */
	int tx_subband3;				/* 5GС�ڸ�Ƶ����õ�����Ƶ */
	int tx_subband4;				/* 5GС�ڸ�Ƶ����õ�����Ƶ1 */
	int tx_subband5;				/* 5GС�ڸ�Ƶ����õ�����Ƶ1 */
	int tx_if_1;
	int tx_if_2;
	int tx_if_3;
	int tx_if_4;
	int tx_if_5;
	int rx_subband1;				/* 5GС�ڸ�Ƶ����õ�һ��Ƶ1 */
	int rx_subband2;                /* 5GС�ڸ�Ƶ����õ�һ��Ƶ2 */
	int rx_subband3;				/* 5GС�ڸ�Ƶ����õڶ���Ƶ */
	int rx_subband4;				/* 5GС�ڸ�Ƶ����õ�����Ƶ */
	int rx_subband5;				/* 5GС�ڸ�Ƶ����õ�����Ƶ1 */
	int rx_subband6;				/* 5GС�ڸ�Ƶ����õ�����Ƶ2 */
	int rx_if_1;
	int rx_if_2;
	int rx_if_3;
	int rx_if_4;
	int rx_if_5;
	int rx_if_6;
	int tx_att_2g4_pa_on_code[TX_ATT_COUNT];
	int tx_att_5g_pa_on_code[TX_ATT_COUNT];
	int tx_att_2g4_pa_off_code[TX_ATT_COUNT];
	int tx_att_5g_pa_off_code[TX_ATT_COUNT];
	int rx_att_2g4_code[RX_ATT_COUNT];
	int rx_att_5g_code[RX_ATT_COUNT];
	int flatness_vsa_version;
	int flatness_vsg_version;
	double temp_subband1;				/* С�ڸ��¶�ʹ�õ�һ�²��ڵ� */
	double temp_subband2;               /* С�ڸ��¶�ʹ�õڶ��²��ڵ� */
	double temp_subband3;				/* С�ڸ��¶�ʹ�õ����²��ڵ�*/
	double temp_subband4;				/* С�ڸ��¶�ʹ�õ����²��ڵ� */
	double cal_port_temp_avg;			/* У׼�˿�ƽ���¶� */
	int  response_data_flag;            /* ƽ̹�����ݸ�ʽ��� */
	int calibration_flat;
} Rf_Config_Data;

extern Rf_Config_Data Rf_Config;
extern Cal_Opt Cal_config;
extern Lo_gain *hmc_lo_gain;

class WTConfig
{
private:
	Csv_disk *p_csv_manager;
	Lo_gain_config *lo_gain_config;
public:
	WTConfig();
	~WTConfig();

	int wt_configuration_initial(char *buf);
	int set_configuration_data(char *filename, char *buf, int len);
	int get_configuration_data(char *filename, char *buf, int bufsize);

	int get_rf_configuration(Rf_Config_Parm *rf_config_parm);
	int set_calibration_option(int buf_size, void *cal_opt);
	int get_calibration_option(int len, void *cal_opt);

private:
	int get_rf_conig_from_buf(char *buf, int len);
	int get_wt_conig_from_buf(char *buf, int len);
};

#endif /* INCLUDE_CONFIGURATION_CONFIG_H_ */
