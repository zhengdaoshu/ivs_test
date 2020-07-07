/*
 * tx_iq_imb.cpp
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */
#include <string.h>
#include <stdlib.h>			//标准函数库定义
#include<stdio.h>
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/hal/cal_data.h"
#include "../../include/hal/rf_set.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/cal_model/tx_iq_imb.h"

Tx_iq_imb::Tx_iq_imb(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_tx_iq_imb *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_tx_iq_imb));
	m_cal_map.clear();
	init_freq();
}

Tx_iq_imb::~Tx_iq_imb()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	    m_cal_map.clear();
	}
}

void Tx_iq_imb::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_tx_iq_imb));
	init_freq();
	m_cal_map.clear();
}

int Tx_iq_imb::wt_calibration_initial(char *buf)
{
	int len;
	int unit = 0;
	int ret = ERR_RET_OK;

	do
	{
		len = p_csv_manager->csv_file_get("VSG/vsg_1_iq_imbalance.csv", buf);
		ret =  get_iq_imb_from_file(buf, len, unit);
		err_break(ret);
		if(m_cal_map.size() <= 0)
		{
			ret = ERR_CAL_TX_IQ_IMB_NULL;
			return ret;
		}
		fill_cal_data(unit);
		unit++;

		m_cal_map.clear();
		len = p_csv_manager->csv_file_get("VSG/vsg_2_iq_imbalance.csv", buf);
		ret = get_iq_imb_from_file(buf, len, unit);
		err_break(ret);
		if(m_cal_map.size() <= 0)
		{
			ret = ERR_CAL_TX_IQ_IMB_NULL;
			return ret;
		}
		fill_cal_data(unit);
		m_cal_map.clear();
	}while(0);
	return ret;
}

double Tx_iq_imb::get_calibration_data(char *ref_buf)
{
	/* 将以Hz 为单位的dfreq 转化为以MHz 为单位的freq */
	Tx_Parm *ret = (Tx_Parm *)ref_buf;
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	int unit_no = get_unit_no(ret->unit);
	ret->tx_iq_imb_parm.gain_imb = m_cal_data[freq_index].tx_iq_imb[unit_no].gain_imb;
	ret->tx_iq_imb_parm.quad_err = m_cal_data[freq_index].tx_iq_imb[unit_no].quad_err;
	ret->tx_iq_imb_parm.timeskew = m_cal_data[freq_index].tx_iq_imb[unit_no].timeskew;
	printf("Tx_iq_imb,ret->unit=%d,freq=%d,gain_imb=%lf,quad_err=%lf,timeskew=%lf\n",ret->unit,freq_index+FREQ_BASE, ret->tx_iq_imb_parm.gain_imb,ret->tx_iq_imb_parm.quad_err,ret->tx_iq_imb_parm.timeskew);
	return 0;
}

int Tx_iq_imb::get_iq_imb_from_file(char *file_buf, int len, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TX_IQ_IMB_CSV_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		ret = App_lib::get_csv_float_item(line, 1, &m_cal_data[freq-FREQ_BASE].tx_iq_imb[unit].gain_imb);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 2, &m_cal_data[freq-FREQ_BASE].tx_iq_imb[unit].quad_err);
		err_continue(ret);
		App_lib::get_csv_float_item(line, 3, &m_cal_data[freq-FREQ_BASE].tx_iq_imb[unit].timeskew);
		m_cal_data[freq-FREQ_BASE].flag[unit] = TRUE;

		if(Rf_set::is_5g(freq))
		{
			m_cal_map[freq] = freq;
		}
		//printf("freq=%d,gain_imb=%lf,quad=%lf,timeskew=%lf\n",freq,m_cal_data[freq-RF_0G_4].tx_iq_imb[unit].gain_imb,m_cal_data[freq-RF_0G_4].tx_iq_imb[unit].quad_err,m_cal_data[freq-RF_0G_4].tx_iq_imb[unit].timeskew);
	}

	return ret;
}

int Tx_iq_imb::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Tx_iq_imb::get_cal_data_by_freq(int freq, int *ch1, int *ch2)
{
	int index = 0;
	int delta_freq = 6000;
	map<int, int>::iterator iter;
	for (iter = m_cal_map.begin(); iter != m_cal_map.end(); iter++)
	{
		if (abs(freq - iter->first) < delta_freq)
		{
			index = iter->first;
			delta_freq = abs(freq - iter->first);
//			if(delta_freq < 5)
//			{
//				break;
//			}
		}
	}
	iter = m_cal_map.find(index);
	if	(freq > index)
	{
		*ch1 = index;
		iter++;
		*ch2 = iter->first;
	}
	else if (freq < index)
	{
		iter--;
		*ch1 = iter->first;
		*ch2 = index;
	}
	else								/* 标准通道 */
	{
		*ch1 = index;
		*ch2 = index;
	}

    //*****************************************************************************
    // ch1  freq  ch2
    //*****************************************************************************
	if(Rf_set::compare_ifg(*ch1, *ch2, CONFIG_TYPE_TX))
	{
		iter = m_cal_map.find(*ch1);
		if(Rf_set::compare_ifg(*ch1, freq, CONFIG_TYPE_TX))
		{
			iter++;
			*ch1 = iter->first;
			iter++;
			*ch2 = iter->first;
		}
		else
		{
			*ch2 = iter->first;
			iter--;
			*ch1 = iter->first;
		}
	}
	return index;
}

int Tx_iq_imb::fill_5g_cal_data(int unit, int freq_index, int CF)
{
	int ch1 = 0, ch1_index = 0;
	int ch2 = 0, ch2_index = 0;
	get_cal_data_by_freq(CF, &ch1, &ch2);
	//printf("unit=%d,ch1=%d,ch2=%d,CF=%d,freq_index=%d\n",unit,ch1,ch2,CF,freq_index);
	ch1_index = ch1 - FREQ_BASE;
	ch2_index = ch2 - FREQ_BASE;
	/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
	m_cal_data[freq_index].flag[unit] = TRUE;
	m_cal_data[freq_index].tx_iq_imb[unit].gain_imb =
			(double)(m_cal_data[ch1_index].tx_iq_imb[unit].gain_imb) +
			(double)(m_cal_data[ch2_index].tx_iq_imb[unit].gain_imb - m_cal_data[ch1_index].tx_iq_imb[unit].gain_imb) /
			(double)(ch2_index - ch1_index) *
			(double)(freq_index - ch1_index);
	m_cal_data[freq_index].tx_iq_imb[unit].quad_err =
			(double)(m_cal_data[ch1_index].tx_iq_imb[unit].quad_err) +
			(double)(m_cal_data[ch2_index].tx_iq_imb[unit].quad_err - m_cal_data[ch1_index].tx_iq_imb[unit].quad_err) /
			(double)(ch2_index - ch1_index) *
			(double)(freq_index - ch1_index);
	m_cal_data[freq_index].tx_iq_imb[unit].timeskew =
			(double)(m_cal_data[ch1_index].tx_iq_imb[unit].timeskew) +
			(double)(m_cal_data[ch2_index].tx_iq_imb[unit].timeskew - m_cal_data[ch1_index].tx_iq_imb[unit].timeskew) /
			(double)(ch2_index - ch1_index) *
			(double)(freq_index - ch1_index);
	//printf("CF=%d,gain_imb=%lf,quad_err=%lf,timeskew=%lf\n",CF,m_cal_data[freq_index].tx_iq_imb[unit].gain_imb,m_cal_data[freq_index].tx_iq_imb[unit].quad_err,m_cal_data[freq_index].tx_iq_imb[unit].timeskew);
	return 0;
}


int Tx_iq_imb::fill_cal_data(int unit)
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
		if(Rf_set::is_5g(m_cal_data[i].freq))
		{
			fill_5g_cal_data(unit, i, m_cal_data[i].freq);
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
			m_cal_data[j].tx_iq_imb[unit].gain_imb =
					(double)(m_cal_data[freq_index1].tx_iq_imb[unit].gain_imb) +
					(double)(m_cal_data[freq_index2].tx_iq_imb[unit].gain_imb - m_cal_data[freq_index1].tx_iq_imb[unit].gain_imb) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
			m_cal_data[j].tx_iq_imb[unit].quad_err =
					(double)(m_cal_data[freq_index1].tx_iq_imb[unit].quad_err) +
					(double)(m_cal_data[freq_index2].tx_iq_imb[unit].quad_err - m_cal_data[freq_index1].tx_iq_imb[unit].quad_err) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
			m_cal_data[j].tx_iq_imb[unit].timeskew =
					(double)(m_cal_data[freq_index1].tx_iq_imb[unit].timeskew) +
					(double)(m_cal_data[freq_index2].tx_iq_imb[unit].timeskew - m_cal_data[freq_index1].tx_iq_imb[unit].timeskew) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
		}
	}

	return 0;
}

