/*
 * rx_const_gain.cpp
 *
 *  Created on: 2017��1��6��
 *      Author: J00054
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>			//��׼�����ⶨ��
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/hal/rf_set.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/cal_model/rx_const_gain.h"
#include "../../include/wt-calibration.h"
#include "../../include/hal/cal_data.h"

Rx_const_gain::Rx_const_gain(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_rx_const_gain *)App_lib::app_malloc((MAX_CHANNEL_COUNT) * sizeof(T_rx_const_gain));
	init_freq();
	m_rx_gain = 0;
}

Rx_const_gain::~Rx_const_gain()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Rx_const_gain::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_rx_const_gain));
	init_freq();
	m_rx_gain = 0;
}

int Rx_const_gain::wt_calibration_initial(char *buf)
{
	int len;
	int unit = 0;
	int ret= ERR_RET_OK;
	do
	{
		len = p_csv_manager->csv_file_get("VSA/vsa_1_gain.csv", buf);
		ret= get_rx_gain_from_buf(buf, len, unit);
		err_break(ret);
		fill_cal_data(unit);
		unit++;
		len = p_csv_manager->csv_file_get("VSA/vsa_2_gain.csv", buf);
		ret= get_rx_gain_from_buf(buf, len, unit);
		err_break(ret);
		fill_cal_data(unit);
	}while(0);
	return ret;
}

double Rx_const_gain::get_calibration_data(char *ref_buf)
{
	Rx_Parm *ret = (Rx_Parm *)ref_buf;
	int	freq = (u32)((ret->freq + MHz/2)/MHz);
	int unit_no = get_unit_no(ret->unit);
	m_rx_gain = m_cal_data[freq-FREQ_BASE].rx_const_gain[unit_no].siso_gain;
	return m_rx_gain;
}

int Rx_const_gain::get_rx_gain_from_buf(char *file_buf, int len, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;

	if (len <= 0)	/* �ļ�ȱʧ�򶼴��쳣 */
	{
		return ERR_RX_CONST_CSV_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		m_cal_data[freq-FREQ_BASE].flag[unit] = TRUE;
		ret = App_lib::get_csv_float_item(line, 1, &m_cal_data[freq-FREQ_BASE].rx_const_gain[unit].siso_gain);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 2, &m_cal_data[freq-FREQ_BASE].rx_const_gain[unit].ac80_gain);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 3, &m_cal_data[freq-FREQ_BASE].rx_const_gain[unit].ac80_link_gain);
		err_continue(ret);
	}
	return ret;
}

int Rx_const_gain::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Rx_const_gain::fill_cal_data(int unit)
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
			/* ��ʽ: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
			m_cal_data[j].flag[unit] = TRUE;
			m_cal_data[j].rx_const_gain[unit].siso_gain =
					(double)(m_cal_data[freq_index1].rx_const_gain[unit].siso_gain) +
					(double)(m_cal_data[freq_index2].rx_const_gain[unit].siso_gain - m_cal_data[freq_index1].rx_const_gain[unit].siso_gain) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
			m_cal_data[j].rx_const_gain[unit].ac80_gain =
					(double)(m_cal_data[freq_index1].rx_const_gain[unit].ac80_gain) +
					(double)(m_cal_data[freq_index2].rx_const_gain[unit].ac80_gain - m_cal_data[freq_index1].rx_const_gain[unit].ac80_gain) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);

			m_cal_data[j].rx_const_gain[unit].ac80_link_gain =
					(double)(m_cal_data[freq_index1].rx_const_gain[unit].ac80_link_gain) +
					(double)(m_cal_data[freq_index2].rx_const_gain[unit].ac80_link_gain - m_cal_data[freq_index1].rx_const_gain[unit].ac80_link_gain) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
		}
	}
	return 0;
}



