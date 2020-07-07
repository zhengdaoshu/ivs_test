/*
 * rx_gain_compensation_port.cpp
 *
 *  Created on: 2017年1月12日
 *      Author: J00054
 */
#include <string.h>
#include <stdlib.h>			//标准函数库定义
#include<stdio.h>
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/hal/rf_set.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/cal_model/rx_gain_compensation_port.h"
#include "../../include/wt-calibration.h"
#include "../../include/hal/cal_data.h"
Rx_gain_compensation_port::Rx_gain_compensation_port(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_rx_gain_compensation *)App_lib::app_malloc((MAX_CHANNEL_COUNT) * sizeof(T_rx_gain_compensation));
	init_freq();
}

Rx_gain_compensation_port::~Rx_gain_compensation_port()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Rx_gain_compensation_port::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_rx_gain_compensation));
	init_freq();
}

int Rx_gain_compensation_port::wt_calibration_initial(char *buf)
{
	int len;
	int port = 0;
	int ret = ERR_RET_OK;
	char csv_file_name[64] = {0};
	for(port = 0; port < VSA_PORT_MAX; port++)
	{
		sprintf(csv_file_name, "VSA/rx_gain_compensation_rf%d.csv", port+1);
		len = p_csv_manager->csv_file_get(csv_file_name, buf);
		ret = get_port_compensation_value_from_buf(buf, len, port);
		fill_cal_data(port);
	}
	return ret;
}

double Rx_gain_compensation_port::get_calibration_data(char *ref_buf)
{
	double m_rx_gain = 0;
	Rx_Parm *ret = (Rx_Parm *)ref_buf;
	int	freq = (u32)((ret->freq + MHz/2)/MHz);
	int gain_index = rx_unit_1_siso;
	int rf_port_index = ret->rf_port;
	if(rf_port_index <= WT_SB_PORT_OFF)
	{
		rf_port_index = WT_SB_PORT_OFF;
	}
	else
	{
		rf_port_index -= 1;
	}

	if(ret->unit == 0)
	{
		gain_index = rx_unit_1_siso;
	}
	else if(ret->unit == 1)
	{
		gain_index = rx_unit_2_siso;
	}
	else if(ret->unit == 2)
	{
		gain_index = rx_unit_1_ac80;
	}
	else if(ret->unit == 3)
	{
		gain_index = rx_unit_1_ac80_link;
	}
	else if(ret->unit == 4)
	{
		gain_index = rx_unit_2_ac80_link;
	}
	else if(ret->unit == 5)
	{
		gain_index = rx_unit_2_ac80;
	}

	m_rx_gain = m_cal_data[freq-FREQ_BASE].rx_gain_compensation[rf_port_index].comp_gain[gain_index];
	printf("Rx_gain_compensation_port,gain_index=%d m_rx_gain=%lf,freq=%d,rf_port_index=%d\n",gain_index,m_rx_gain,freq,rf_port_index);
	return m_rx_gain;
}

int Rx_gain_compensation_port::get_port_compensation_value_from_buf(char *file_buf, int len, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int i;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_RX_COMP_CSV_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		m_cal_data[freq-FREQ_BASE].flag[port] = TRUE;
		for(i=0; i<RX_COMP_COLOUM; i++)
		{
			ret = App_lib::get_csv_float_item(line, i+1, &m_cal_data[freq-FREQ_BASE].rx_gain_compensation[port].comp_gain[i]);
			err_break(ret);
		}
	}
	return ret;
}

int Rx_gain_compensation_port::init_freq()
{
	int i = 0;
	for(i = 0; i < MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Rx_gain_compensation_port::fill_cal_data(int port)
{
	int i = 0;
	int j = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_cal_data[i].flag[port])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_cal_data[j].flag[port])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_cal_data[j].flag[port] && j != freq_index1)
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
			m_cal_data[j].flag[port] = TRUE;
			int k;
			for (k=0; k<RX_COMP_COLOUM; k++)
			{
				/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
				m_cal_data[j].rx_gain_compensation[port].comp_gain[k] =
						(double)(m_cal_data[freq_index1].rx_gain_compensation[port].comp_gain[k]) +
						(double)(m_cal_data[freq_index2].rx_gain_compensation[port].comp_gain[k] - m_cal_data[freq_index1].rx_gain_compensation[port].comp_gain[k]) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1);
			}
		}
	}
	return 0;
}


