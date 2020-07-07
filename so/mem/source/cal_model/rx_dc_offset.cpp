/*
 * rx_dc_offset.cpp
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "../../include/base/app_lib.h"
#include "../../include/cal_model/rx_dc_offset.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/hal/cal_data.h"

Rx_dc_offset::Rx_dc_offset(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_rx_dc_offset *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_rx_dc_offset));
	init_freq();
}

Rx_dc_offset::~Rx_dc_offset()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Rx_dc_offset::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_rx_dc_offset));
	init_freq();
}

int Rx_dc_offset::wt_calibration_initial(char *buf)
{
	int len;
	int unit = 0;
	int ret = ERR_RET_OK;
	len = p_csv_manager->csv_file_get("VSA/vsa_1_dc_offset.csv", buf);
	ret = get_rx_dc_offset_from_buf(buf, len, unit);
	fill_cal_data(unit);
	unit++;

	len = p_csv_manager->csv_file_get("VSA/vsa_2_dc_offset.csv", buf);
	ret = get_rx_dc_offset_from_buf(buf, len, unit);
	fill_cal_data(unit);
	return ret;
}

double Rx_dc_offset::get_calibration_data(char *ref_buf)
{
	Rx_Parm *ret = (Rx_Parm *)ref_buf;
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	int unit_no = get_unit_no(ret->unit);
	ret->rx_dc_offset_parm.i_code = m_cal_data[freq_index].rx_dc_offset[unit_no].i_code;
	ret->rx_dc_offset_parm.q_code = m_cal_data[freq_index].rx_dc_offset[unit_no].q_code;
	printf("freq=%d,ret->unit=%d,rx_dc_offset_parm,i=%d,q=%d\n",freq_index+FREQ_BASE,ret->unit,m_cal_data[freq_index].rx_dc_offset[unit_no].i_code,m_cal_data[freq_index].rx_dc_offset[unit_no].q_code);
	return 0;
}

int Rx_dc_offset::get_rx_dc_offset_from_buf(char *file_buf, int len, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_RX_DC_OFFSET_CSV_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0) || (strncmp(line, "Interval", 8) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		ret = App_lib::get_csv_decimal_item(line, 1, &m_cal_data[freq-FREQ_BASE].rx_dc_offset[unit].i_code);
		err_continue(ret);
		ret = App_lib::get_csv_decimal_item(line, 2, &m_cal_data[freq-FREQ_BASE].rx_dc_offset[unit].q_code);
		err_continue(ret);
		m_cal_data[freq-FREQ_BASE].flag[unit] = TRUE;
	}
	return ret;
}

int Rx_dc_offset::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Rx_dc_offset::fill_cal_data(int unit)
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
			m_cal_data[j].rx_dc_offset[unit].i_code =
					(int)((double)(m_cal_data[freq_index1].rx_dc_offset[unit].i_code) +
					(double)(m_cal_data[freq_index2].rx_dc_offset[unit].i_code - m_cal_data[freq_index1].rx_dc_offset[unit].i_code) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1));
			m_cal_data[j].rx_dc_offset[unit].q_code =
					(int)((double)(m_cal_data[freq_index1].rx_dc_offset[unit].q_code) +
					(double)(m_cal_data[freq_index2].rx_dc_offset[unit].q_code - m_cal_data[freq_index1].rx_dc_offset[unit].q_code) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1));
		}
	}
	return 0;
}
