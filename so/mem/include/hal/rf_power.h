/*
 * rf_power.h
 *
 *  Created on: 2016Äê12ÔÂ14ÈÕ
 *      Author: J00054
 */

#ifndef SOURCE_HAL_RF_POWER_H_
#define SOURCE_HAL_RF_POWER_H_
#include "../cal_model/cal_model.h"
#include "../cal_model/att_vsa.h"
#include "../cal_model/att_vsg.h"
#include "../cal_model/tx_const_gain.h"
#include "../cal_model/rx_const_gain.h"
#include "../cal_model/tx_gain_compensation_port.h"
#include "../cal_model/rx_gain_compensation_port.h"
#include "../cal_model/tx_response.h"
#include "../cal_model/rx_response.h"
#include "../cal_model/temp_compensation.h"
#include "../wt-calibration.h"
class Rf_power
{
private:
	Tx_const_gain* tx_const_gain;
	Att_vsg* att_vsg;
	Rx_const_gain* rx_const_gain;
	Att_vsa* att_vsa;
	Temp_config* temp_config;
	Tx_gain_compensation_port *tx_gain_compensation_port;
	Rx_gain_compensation_port *rx_gain_compensation_port;
	Cal_model *tx_response_model;
	Cal_model *rx_response_model;

public:
	Rf_power(Cal_model* cal_model[]);
	~Rf_power(){};
	int get_tx_gain_setting(Tx_Parm *tx_parm, double sample_freq);
	int get_rx_gain_setting(Rx_Parm *rx_parm, double sample_freq);
};

#endif /* SOURCE_HAL_RF_POWER_H_ */
