/*
 * rf_power.cpp
 *
 *  Created on: 2016Äê12ÔÂ14ÈÕ
 *      Author: J00054
 */
#include <stdio.h>
#include "../../include/hal/rf_power.h"
#include "../../include/hal/rf_set.h"
#include "../../include/base/typedef.h"
#include "../../include/config/config.h"
Rf_power::Rf_power(Cal_model* cal_model[])
{
	tx_const_gain = (Tx_const_gain*)(cal_model[CalTxConstGain]);
	att_vsg = (Att_vsg*)(cal_model[CalTxAtt]);
	rx_const_gain = (Rx_const_gain*)(cal_model[CalRxConstGain]);
	att_vsa = (Att_vsa*)(cal_model[CalRxAtt]);
	temp_config = (Temp_config*)(cal_model[CalTEMPERATURE]);
	tx_gain_compensation_port = (Tx_gain_compensation_port*)(cal_model[CalTxCompensationGain]);
	rx_gain_compensation_port = (Rx_gain_compensation_port*)(cal_model[CalRxCompensationGain]);
	tx_response_model = cal_model[CalTxResponse];
	rx_response_model = cal_model[CalRxResponse];
}

int Rf_power::get_tx_gain_setting(Tx_Parm *tx_parm, double sample_freq)
{
	double tx_pa_threshold = -30.0;
	double tx_gain = 0;
	double const_gain = 0;
	int	freq = (u32)((tx_parm->freq + MHz/2)/MHz);
	if (Rf_set::is_5g(freq))
	{
		tx_pa_threshold = Rf_Config.tx_pa_threshold_5g;
	}
	else
	{
		tx_pa_threshold = Rf_Config.tx_pa_threshold_2g;
	}
	if (tx_parm->power >= tx_pa_threshold)
	{
		tx_parm->tx_gain_parm.is_pa_on = TRUE;
	}
	else
	{
		tx_parm->tx_gain_parm.is_pa_on = FALSE;
	}
	tx_gain = tx_const_gain->get_calibration_data((char*)tx_parm) + tx_gain_compensation_port->get_calibration_data((char*)tx_parm);
	//tx_gain = tx_const_gain->get_calibration_data((char*)tx_parm);
	const_gain = temp_config->get_tx_compensation(tx_gain, (char*)tx_parm);
	printf("tx_gain=%lf,after tempcompensation const_gain=%lf\n",tx_gain, const_gain);
	att_vsg->set_tx_const_gain(const_gain);
	att_vsg->get_calibration_data((char*)tx_parm);

	if (Rf_Config.flatness_vsa_version)
	{
		((Tx_response_v2*)tx_response_model)->set_tx_sample_freq(sample_freq);
	}
	else
	{
		((Tx_response*)tx_response_model)->set_tx_sample_freq(sample_freq);
	}
	tx_response_model->get_calibration_data((char*)tx_parm);

	return 0;
}

int Rf_power::get_rx_gain_setting(Rx_Parm *rx_parm, double sample_freq)
{
	double rx_gain = 0;
	double const_gain = 0;

	rx_gain = rx_const_gain->get_calibration_data((char*)rx_parm) + rx_gain_compensation_port->get_calibration_data((char*)rx_parm);
	const_gain = temp_config->get_rx_compensation(rx_gain, (char*)rx_parm);
	printf("rx_gain=%lf,after tempcompensation const_gain=%lf\n",rx_gain, const_gain);
	att_vsa->set_rx_const_gain(const_gain);
	if (Rf_Config.calibration_flat)
	{
		att_vsa->get_cal_mode_data((char*)rx_parm);
	}
	else
	{
		att_vsa->get_calibration_data((char*)rx_parm);
	}

	if (Rf_Config.flatness_vsa_version)
	{
		((Rx_response_v2*)rx_response_model)->set_rx_sample_freq(sample_freq);
	}
	else
	{
		((Rx_response*)rx_response_model)->set_rx_sample_freq(sample_freq);
	}

	rx_response_model->get_calibration_data((char*)rx_parm);

	return 0;
}


