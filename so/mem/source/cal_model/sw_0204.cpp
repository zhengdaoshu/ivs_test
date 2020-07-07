/*
 * sw_0204.cpp
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */
#include<string.h>
#include<stdlib.h>			//标准函数库定义
#include "../../include/cal_model/sw_0204.h"
#include "../../include/wt-calibration.h"
#include "../../include/hal/cal_data.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/base/app_lib.h"
Sw_0204_loss::Sw_0204_loss(int csv_type) :Cal_model(csv_type)
{
	m_src_type = csv_type;
	m_cal_data = (T_sw_0204 *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_sw_0204));
	init_freq();
}

Sw_0204_loss::~Sw_0204_loss()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Sw_0204_loss::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_sw_0204));
	init_freq();
}

int Sw_0204_loss::wt_calibration_initial(char *buf)
{
	int len;
	int unit = 0;
	int ret = ERR_RET_OK;
	do
	{
		len = p_csv_manager->csv_file_get("SW0204/sw_1_0204_insertion_loss.csv", buf);
		ret = get_sw0204_from_buf(buf, len, unit);
		err_break(ret);
		fill_cal_data(unit);
		unit++;

		len = p_csv_manager->csv_file_get("SW0204/sw_2_0204_insertion_loss.csv", buf);
		ret = get_sw0204_from_buf(buf, len, unit);
		err_break(ret);
		fill_cal_data(unit);
	}while(0);
	return ret;
}

double Sw_0204_loss::get_calibration_data(char *ref_buf)
{
	return 0;
}

int Sw_0204_loss::get_sw0204_from_buf(char *file_buf, int len, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int j;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_SW_0204_CSV_LOST;
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
		for (j=0; j<SW0204_COLUMN; j++)
		{
			ret = App_lib::get_csv_float_item(line, j + 1, &m_cal_data[freq-FREQ_BASE].loss[unit].loss_value[j]);
			err_break(ret);
		}
	}

	return ret;
}

int Sw_0204_loss::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Sw_0204_loss::fill_cal_data(int unit)
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
			m_cal_data[j].flag[unit] = TRUE;
			int k;
			for (k=0; k<SW0204_COLUMN; k++)
			{
				/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
				m_cal_data[j].loss[unit].loss_value[k] =
						(double)(m_cal_data[freq_index1].loss[unit].loss_value[k]) +
						(double)(m_cal_data[freq_index2].loss[unit].loss_value[k] - m_cal_data[freq_index1].loss[unit].loss_value[k]) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1);
			}
		}
	}
	return 0;
}
