/*
 * tx_dc_offset.cpp
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../include/cal_model/tx_dc_offset.h"
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/hal/rf_set.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/hal/cal_data.h"

Tx_dc_offset::Tx_dc_offset(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_tx_dc_offset *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_tx_dc_offset));
	init_freq();
}

Tx_dc_offset::~Tx_dc_offset()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Tx_dc_offset::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_tx_dc_offset));
	init_freq();
}

int Tx_dc_offset::wt_calibration_initial(char *buf)
{
	int len;
	int unit = 0;
	int ret = ERR_RET_OK;
	len = p_csv_manager->csv_file_get("VSG/vsg_1_dc_offset.csv", buf);
	get_tx_dc_offset_from_file(buf, len, unit);
	fill_cal_data(unit);
	unit++;

	len = p_csv_manager->csv_file_get("VSG/vsg_2_dc_offset.csv", buf);
	ret = get_tx_dc_offset_from_file(buf, len, unit);
	fill_cal_data(unit);
	return ret;
}

double Tx_dc_offset::get_calibration_data(char *ref_buf)
{
	Tx_Parm* ret = (Tx_Parm*)ref_buf;
	int	freq = (u32)((ret->freq + MHz/2)/MHz);
	int unit_no = get_unit_no(ret->unit);
	printf("get_tx_dc_offset,unit=%d,freq=%d,dac_gain=%lf\n",ret->unit,freq,ret->tx_gain_parm.dac_gain);
	get_tx_dc_offset(unit_no, freq, ret->tx_gain_parm.dac_gain, &(ret->tx_dc_offset_parm));
	return 0;
}

int Tx_dc_offset::get_tx_dc_offset_from_file(char *file_buf, int len, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int j;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TX_DC_OFFSET_CSV_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		for (j=0; j<TX_DC_OFFSET_MAX_COUNT; j++)
		{
			ret = App_lib::get_csv_decimal_item(line, 2*j+1, &m_cal_data[freq-FREQ_BASE].tx_dc_offset[unit].dc_offset[j].i_code);
			err_break(ret);
			ret = App_lib::get_csv_decimal_item(line, 2*j+2, &m_cal_data[freq-FREQ_BASE].tx_dc_offset[unit].dc_offset[j].q_code);
			err_break(ret);
		}
		m_cal_data[freq-FREQ_BASE].flag[unit] = TRUE;
	}
	return ret;
}

int Tx_dc_offset::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Tx_dc_offset::fill_cal_data(int unit)
{
	int i = 0;
	int j = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_cal_data[i].flag[unit])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_cal_data[j].flag[unit])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_cal_data[j].flag[unit] && j != freq_index1)
			{
				freq_index2 = j;
				break;
			}
		}
		if(i < freq_index1)
		{
			min_index = i;
			max_index = freq_index1;
		}
		else
		{
			min_index = freq_index1 + 1;
			max_index = freq_index2;
		}
		for(j = min_index; j < max_index; j++)
		{
			/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
			m_cal_data[j].flag[unit] = TRUE;
			int k;
			for (k=0; k<TX_DC_OFFSET_MAX_COUNT; k++)
			{
				/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
				m_cal_data[j].tx_dc_offset[unit].dc_offset[k].i_code =
						(int)((double)(m_cal_data[freq_index1].tx_dc_offset[unit].dc_offset[k].i_code) +
						(double)(m_cal_data[freq_index2].tx_dc_offset[unit].dc_offset[k].i_code - m_cal_data[freq_index1].tx_dc_offset[unit].dc_offset[k].i_code) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1));
				m_cal_data[j].tx_dc_offset[unit].dc_offset[k].q_code =
						(int)((double)(m_cal_data[freq_index1].tx_dc_offset[unit].dc_offset[k].q_code) +
						(double)(m_cal_data[freq_index2].tx_dc_offset[unit].dc_offset[k].q_code - m_cal_data[freq_index1].tx_dc_offset[unit].dc_offset[k].q_code) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1));
			}
		}
	}
	return 0;
}

int Tx_dc_offset::get_tx_dc_offset(int unit, int freq, double dac_gain, Tx_DC_Offset_Parm* ref_tx_dc_offset)
{
	int ch1 = 0;
	int ch2 = 0;
	int i;
	double gain_step[TX_DC_OFFSET_MAX_COUNT] = {-0.5, 0, 0.5};
	double delta_gain = 2;

	/* 找出最接近的两个gain点 */
	for (i=0; i<TX_DC_OFFSET_MAX_COUNT; i++)
	{
		double temp;
		if (gain_step[i] > dac_gain)
			temp = gain_step[i] - dac_gain;
		else
			temp = dac_gain - gain_step[i];
		if (temp < delta_gain)
		{
			delta_gain = temp;
			ch1 = i;
		}
	}
	delta_gain = 2;
	for (i=0; i<TX_DC_OFFSET_MAX_COUNT; i++)
	{
		double temp;
		if (gain_step[i] > dac_gain)
			temp = gain_step[i] - dac_gain;
		else
			temp = dac_gain - gain_step[i];
		if ((temp < delta_gain) && (i != ch1))
		{
			delta_gain = temp;
			ch2 = i;
		}
	}
	ref_tx_dc_offset->i_code = (int)((double)m_cal_data[freq - FREQ_BASE].tx_dc_offset[unit].dc_offset[ch1].i_code +
		(double)(m_cal_data[freq - FREQ_BASE].tx_dc_offset[unit].dc_offset[ch2].i_code - m_cal_data[freq - FREQ_BASE].tx_dc_offset[unit].dc_offset[ch1].i_code) /
		(double)(gain_step[ch2] - gain_step[ch1]) *
		(double)(dac_gain - gain_step[ch1]));
	ref_tx_dc_offset->q_code = (int)((double)m_cal_data[freq - FREQ_BASE].tx_dc_offset[unit].dc_offset[ch1].q_code +
		(double)(m_cal_data[freq - FREQ_BASE].tx_dc_offset[unit].dc_offset[ch2].q_code - m_cal_data[freq - FREQ_BASE].tx_dc_offset[unit].dc_offset[ch1].q_code) /
		(double)(gain_step[ch2] - gain_step[ch1]) *
		(double)(dac_gain - gain_step[ch1]));
	return 0;
}
