/*******************************************************************************
*
* file name: wt-calibration.cpp
* descript:
* create by: liweidong 2016.10.17
*
*******************************************************************************/
#define WT_CALIBRATION_CPP
#include <stdlib.h>
#include <string.h>
#include "../include/wt-calibration.h"
#include "../include/cal_data_manager.h"
#include "../include/base/typedef.h"
#include "../include/base/cal_error.h"

Cal_data_manager* p_cal_data_manager = NULL;

inline BOOL wt_assert(Cal_data_manager* p_cal_manager)
{
	if(p_cal_manager == NULL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int wt_calibration_initial()
{
	int ret;
	if(!wt_assert(p_cal_data_manager))
	{
		p_cal_data_manager = new Cal_data_manager();
	}
	ret = p_cal_data_manager->wt_calibration_initial();
	return ret;
}

const char* wt_calibration_version()
{
	return CAL_VERSION;
}

int wt_calibration_get_ocxo_setting(Ocxo_Parm *ocxo_parm)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	p_cal_data_manager->get_ocxo_code(ocxo_parm);
	return 0;
}

int wt_calibration_get_tx_setting(Tx_Parm *tx_parm, double sample_freq)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->get_tx_setting(tx_parm, sample_freq);
}

int wt_calibration_get_rx_setting(Rx_Parm *rx_parm, double sample_freq)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->get_rx_setting(rx_parm, sample_freq);
}

int wt_calibration_get_configuration(Rf_Config_Parm *rf_config_parm)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	p_cal_data_manager->get_configuration(rf_config_parm);
	return 0;
}

int wt_calibration_get_calibration_data(char *filename, char *buf, int bufsize, int len)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->get_calibration_data(filename, buf, bufsize);
}

int wt_calibration_set_calibration_data(char *filename, char *buf, int len)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->set_calibration_data(filename, buf, len);
}

int wt_calibration_get_calibration_configuration_data(char *filename, char *buf, int bufsize, int len)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->get_configuration_data(filename, buf, bufsize);
}

int wt_calibration_set_calibration_configuration_data(char *filename, char *buf, int len)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->set_configuration_data(filename, buf, len);
}

int wt_calibration_set_calibration_option(int buf_size, void *cal_opt)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->set_calibration_option(buf_size, cal_opt);
}

int wt_calibration_get_calibration_option(int len, void *cal_opt)
{
	if(!wt_assert(p_cal_data_manager))
	{
		return ERR_CAL_NOT_INIT;
	}
	return p_cal_data_manager->get_calibration_option(len, cal_opt);
}

