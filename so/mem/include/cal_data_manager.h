/*
 * cal_data_manager.h
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_DATA_MANAGER_H_
#define INCLUDE_CAL_DATA_MANAGER_H_

#include "cal_model/cal_model.h"
#include "config/config.h"
#include "hal/rf_power.h"
#include "wt-calibration.h"

#define CAL_VERSION "1.2.0.23"	/* 校准数据管理模块版本 */

class Cal_data_manager
{
private:
	Cal_model* m_cal_model[CAL_MAX_NUM];
	WTConfig* m_wtconfig;
	Rf_power* rf_power;
	int m_cal_src_type;  //校准数据来源类型，disk、flash
public:
	Cal_data_manager();
	~Cal_data_manager();

	int wt_calibration_initial();
	int wt_config_initial();

	int set_calibration_data(char *filename, char *buf, int len);
	int get_calibration_data(char *filename, char *buf, int size);
	int set_configuration_data(char *filename, char *buf, int len);
	int get_configuration_data(char *filename, char *buf, int size);

	int get_ocxo_code(Ocxo_Parm *ocxo_parm);

	int get_configuration(Rf_Config_Parm *rf_config_parm);
	int set_calibration_option(int buf_size, void *cal_opt);
	int get_calibration_option(int len, void *cal_opt);

	int get_tx_setting(Tx_Parm *tx_parm, double sample_freq);

	int get_rx_setting(Rx_Parm *rx_parm, double sample_freq);

private:
	int init_cal_modeles();
};

#endif /* INCLUDE_CAL_DATA_MANAGER_H_ */
