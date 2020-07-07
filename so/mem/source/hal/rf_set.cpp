/*
 * rf_set.cpp
 *
 *  Created on: 2016年11月25日
 *      Author: J00054
 */
#include<stdio.h>
#include "../../include/hal/cal_data.h"
#include "../../include/base/typedef.h"
#include "../../include/hal/rf_set.h"
#include "../../include/cal_model/cal_model.h"
#include "../../include/config/config.h"
#include "../../include/base/cal_error.h"
#include "../../include/base/app_lib.h"

static int unit_map[6] = {0};

int Rf_set::is_5g(int freq)
{
	if ((freq >= RF_5G_MIN) && (freq <= RF_5G_MAX))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int Rf_set::is_2g4(int freq)
{
	if ((freq >= RF_2G_4_MIN) && (freq <= RF_2G_4_MAX))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Rf_set::init_unit_map()
{
	unit_map[0] = 0;
	unit_map[1] = 1;
	unit_map[2] = 0;
	unit_map[3] = 1;
	unit_map[4] = 0;
	unit_map[5] = 1;
}

int Rf_set::get_unit_no(int unit_value)
{
	if(unit_map[1] == 0)
	{
		init_unit_map();
	}
	if(unit_value > 5 || unit_value < 0)
	{
		return 0;
	}
	return unit_map[unit_value];
}

int Rf_set::is_valid_freq(double dFreq)
{
	int freq = (int)((dFreq + MHz/2)/MHz);
	if(freq > RF_5G_MAX || freq < FREQ_BASE)
	{
		return ERR_FREQ_INVALID;
	}
	else
	{
		return ERR_RET_OK;
	}
}

int Rf_set::get_rx_lo_setting(int unit, double dFreq, Freq_Parm *freq_parm)
{
	if(hmc_lo_gain == NULL)
	{
		return ERR_LO_GAIN_NOT_INIT;
	}
	int freq = (int)((dFreq + MHz/2)/MHz);
	int unit_no = get_unit_no(unit);
	if (is_5g(freq))
	{
		int if_5g = 0;
		if (Rf_Config.rx_subband6 != 0) //VB板
		{
			if(freq < Rf_Config.rx_subband1)
			{
				if_5g = Rf_Config.rx_if_1;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_1;
			}
			else if(freq < Rf_Config.rx_subband2)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_2, 50);
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_1;
			}
			else if(freq < Rf_Config.rx_subband3)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_3, 50);
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_2;
			}
			else if(freq < Rf_Config.rx_subband4)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_4, 50);
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_3;
			}
			else if(freq < Rf_Config.rx_subband5)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_5, 50);
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
			}
			else
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_6, 50);
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
			}

			freq_parm->LoParm[LO_HMC833].freq = freq - if_5g;
		}
		else //VA
		{
			if(freq < Rf_Config.rx_subband1)
			{
				if_5g = Rf_Config.rx_if_1 * 2;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_1;
			}
			else if(freq < Rf_Config.rx_subband2)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_2, 50) * 2;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_2;
			}
			else if(freq < Rf_Config.rx_subband3)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_3, 50) * 2;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_3;
			}
			else if(freq < Rf_Config.rx_subband4)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_4, 50) * 2;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
			}
			else if(freq < Rf_Config.rx_subband5)
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_5, 50) * 2;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
			}
			else
			{
				if_5g = App_lib::near_num(freq - Rf_Config.rx_if_6, 50) * 2;
				freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
			}

			freq_parm->LoParm[LO_HMC833].freq = freq - (int)(if_5g / 2);
		}

		freq_parm->LoParm[LO_HMC833].lo = LO_HMC833;
		freq_parm->LoParm[LO_HMC833].power_level = hmc_lo_gain[freq-FREQ_BASE].value[2+unit_no];

		freq_parm->LoParm[LO_HMC767].freq = if_5g;
		freq_parm->LoParm[LO_HMC767].lo = LO_HMC767;
		freq_parm->LoParm[LO_HMC767].power_level = 0;
	}
	else
	{
		freq_parm->BandMode = WT_RF_BAND_2_4G;

		freq_parm->LoParm[LO_HMC833].freq = (double)freq;
		freq_parm->LoParm[LO_HMC833].lo = LO_HMC833;
		freq_parm->LoParm[LO_HMC833].power_level = hmc_lo_gain[freq-FREQ_BASE].value[2+unit_no];

		freq_parm->LoParm[LO_HMC767].freq = 0;
		freq_parm->LoParm[LO_HMC767].lo = LO_HMC767;
		freq_parm->LoParm[LO_HMC767].power_level = 0;
	}
	return 0;
}

int Rf_set::get_tx_lo_setting(int unit, double dFreq, Freq_Parm *freq_parm)
{
	if(hmc_lo_gain == NULL)
	{
		return ERR_LO_GAIN_NOT_INIT;
	}
	int freq = (int)((dFreq + MHz/2)/MHz);
	int unit_no = get_unit_no(unit);
	if (is_5g(freq))
	{
		int if_5g = 0;
		if(freq < Rf_Config.tx_subband1)
		{
			if_5g = Rf_Config.tx_if_1 * 2;
			freq_parm->BandMode = WT_RF_5G_SUB_BAND_1;
		}
		else if(freq < Rf_Config.tx_subband2)
		{
			if_5g = Rf_Config.tx_if_2 * 2;
			freq_parm->BandMode = WT_RF_5G_SUB_BAND_2;
		}
		else if(freq < Rf_Config.tx_subband3)
		{
			if_5g = Rf_Config.tx_if_3 * 2;
			freq_parm->BandMode = WT_RF_5G_SUB_BAND_3;
		}
		else if(freq < Rf_Config.tx_subband4)
		{
			if_5g = Rf_Config.tx_if_4 * 2;
			freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
		}
		else
		{
			if_5g = Rf_Config.tx_if_5 * 2;
			freq_parm->BandMode = WT_RF_5G_SUB_BAND_4;
		}
		freq_parm->LoParm[LO_HMC833].freq = freq - (int)(if_5g / 2);
		freq_parm->LoParm[LO_HMC833].lo = LO_HMC833;
		freq_parm->LoParm[LO_HMC833].power_level = hmc_lo_gain[freq-FREQ_BASE].value[unit_no];

		freq_parm->LoParm[LO_HMC767].freq = if_5g;
		freq_parm->LoParm[LO_HMC767].lo = LO_HMC767;
		freq_parm->LoParm[LO_HMC767].power_level = 0;
	}
	else
	{
		freq_parm->BandMode = WT_RF_BAND_2_4G;

		freq_parm->LoParm[LO_HMC833].freq = (double)freq;
		freq_parm->LoParm[LO_HMC833].lo = LO_HMC833;
		freq_parm->LoParm[LO_HMC833].power_level = hmc_lo_gain[freq-FREQ_BASE].value[unit_no];

		freq_parm->LoParm[LO_HMC767].freq = 0;
		freq_parm->LoParm[LO_HMC767].lo = LO_HMC767;
		freq_parm->LoParm[LO_HMC767].power_level = 0;
	}
	return 0;
}

// 0 同一个中频
// 1 不同在一个中频
int Rf_set::compare_ifg(int freq1, int freq2, int mode)
{
	int ret = 0;
	int if_5g = 0;
	int if_5g_index1 = 0, if_5g_index2 = 0;
	if(mode == CONFIG_TYPE_RX)
	{
		if_5g_index1 = get_rx_if_5g(freq1, &if_5g);
		if_5g_index2 = get_rx_if_5g(freq2, &if_5g);
		if(if_5g_index1 == if_5g_index2)
		{
			ret = 0;
		}
		else
		{
			ret = 1;
		}
	}
	else
	{
		if_5g_index1 = get_tx_if_5g(freq1, &if_5g);
		if_5g_index2 = get_tx_if_5g(freq2, &if_5g);
		if(if_5g_index1 == if_5g_index2)
		{
			ret = 0;
		}
		else
		{
			ret = 1;
		}
	}
	return ret;
}

int Rf_set::get_tx_if_5g(int freq, int *if_5g)
{
	if(freq < Rf_Config.tx_subband1)
	{
		*if_5g = Rf_Config.tx_subband1;
		return IF_5G_1;
	}
	else if(freq < Rf_Config.tx_subband2)
	{
		*if_5g = Rf_Config.tx_subband2;
		return IF_5G_2;
	}
	else if(freq < Rf_Config.tx_subband3)
	{
		*if_5g = Rf_Config.tx_subband3;
		return IF_5G_3;
	}
	else if(freq < Rf_Config.tx_subband4)
	{
		*if_5g = Rf_Config.tx_subband4;
		return IF_5G_4;
	}
	else
	{
		*if_5g = Rf_Config.tx_subband5;
		return IF_5G_5;
	}
	return 0;
}

int Rf_set::get_rx_if_5g(int freq, int *if_5g)
{
	if(freq < Rf_Config.rx_subband1)
	{
		*if_5g = Rf_Config.rx_subband1;
		return IF_5G_1;
	}
	else if(freq < Rf_Config.rx_subband2)
	{
		*if_5g = Rf_Config.rx_subband2;
		return IF_5G_2;
	}
	else if(freq < Rf_Config.rx_subband3)
	{
		*if_5g = Rf_Config.rx_subband3;
		return IF_5G_3;
	}
	else if(freq < Rf_Config.rx_subband4)
	{
		*if_5g = Rf_Config.rx_subband4;
		return IF_5G_4;
	}
	else if(freq < Rf_Config.rx_subband5)
	{
		*if_5g = Rf_Config.rx_subband5;
		return IF_5G_5;
	}
	else
	{
		*if_5g = Rf_Config.rx_subband6;
		return IF_5G_6;
	}
	return 0;
}

int Rf_set::get_rx_if_5g_by_index(int if_5g_index)
{
	if(if_5g_index ==  IF_5G_1)
	{
		return Rf_Config.rx_subband1;
	}
	else if(if_5g_index ==  IF_5G_2)
	{
		return Rf_Config.rx_subband2;
	}
	else if(if_5g_index ==  IF_5G_3)
	{
		return Rf_Config.rx_subband3;
	}
	else if(if_5g_index ==  IF_5G_4)
	{
		return Rf_Config.rx_subband4;
	}
	else if(if_5g_index ==  IF_5G_5)
	{
		return Rf_Config.rx_subband5;
	}
	else if(if_5g_index ==  IF_5G_6)
	{
		return Rf_Config.rx_subband6;
	}

	return Rf_Config.rx_subband2;
}

int Rf_set::get_tx_if_5g_by_index(int if_5g_index)
{
	if(if_5g_index ==  IF_5G_1)
	{
		return Rf_Config.tx_subband1;
	}
	else if(if_5g_index ==  IF_5G_2)
	{
		return Rf_Config.tx_subband2;
	}
	else if(if_5g_index ==  IF_5G_3)
	{
		return Rf_Config.tx_subband3;
	}
	else if(if_5g_index ==  IF_5G_4)
	{
		return Rf_Config.tx_subband4;
	}
	else if(if_5g_index ==  IF_5G_5)
	{
		return Rf_Config.tx_subband5;
	}
	return Rf_Config.tx_subband2;
}

