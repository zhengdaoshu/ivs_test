/*
 * conf.h
 *
 *  Created on: 2016年12月13日
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
	int tx_temp_cps;                /* tx温度补偿开关 */
	int rx_temp_cps;
	int tx_subband1;				/* 5G小于该频点采用第一中频 */
	int tx_subband2;                /* 5G小于该频点采用第二中频 */
	int tx_subband3;				/* 5G小于该频点采用第三中频 */
	int tx_subband4;				/* 5G小于该频点采用第四中频1 */
	int tx_subband5;				/* 5G小于该频点采用第四中频1 */
	int tx_if_1;
	int tx_if_2;
	int tx_if_3;
	int tx_if_4;
	int tx_if_5;
	int rx_subband1;				/* 5G小于该频点采用第一中频1 */
	int rx_subband2;                /* 5G小于该频点采用第一中频2 */
	int rx_subband3;				/* 5G小于该频点采用第二中频 */
	int rx_subband4;				/* 5G小于该频点采用第三中频 */
	int rx_subband5;				/* 5G小于该频点采用第四中频1 */
	int rx_subband6;				/* 5G小于该频点采用第四中频2 */
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
	double temp_subband1;				/* 小于该温度使用第一温补节点 */
	double temp_subband2;               /* 小于该温度使用第二温补节点 */
	double temp_subband3;				/* 小于该温度使用第三温补节点*/
	double temp_subband4;				/* 小于该温度使用第四温补节点 */
	double cal_port_temp_avg;			/* 校准端口平均温度 */
	int  response_data_flag;            /* 平坦度数据格式标记 */
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
