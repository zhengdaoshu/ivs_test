/*
 * att.cpp
 *
 *  Created on: 2016年11月29日
 *      Author: J00054
 */
#include<string.h>
#include<stdlib.h>			//标准函数库定义
#include<stdio.h>
#include<math.h>
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/cal_model/att_vsg.h"
#include "../../include/hal/cal_data.h"
#include "../../include/hal/rf_set.h"
#include "../../include/config/config.h"
Att_vsg::Att_vsg(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_tx_att *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_tx_att));
	init_freq();
	m_const_gain = 0.0;
	share_att4_code = 0;
}

Att_vsg::~Att_vsg()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Att_vsg::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_tx_att));
	init_freq();
	m_const_gain = 0.0;
	share_att4_code = 0;
}

int Att_vsg::wt_calibration_initial(char *buf)
{
	int ret = ERR_RET_OK;
	ret = get_att_pa_on_calibration_data(buf);
	get_att_off_calibration_data();
	return ret;
}

double Att_vsg::get_calibration_data(char *ref_buf)
{
	double tune_gain = 0;	/* 需要的可调增益 */
	Tx_Parm *ret = (Tx_Parm *)ref_buf;
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	tune_gain = ret->power - m_const_gain;
	tune_gain += (Rf_Config.dac_margin_ax - Rf_Config.dac_margin); //VB板测试用
	int unit_no = get_unit_no(ret->unit);
	int share_unit = 0;
	printf("ret->unit=%d,unit_no=%d,ret->power=%lf,tune_gain=%lf,deta_dac_margin=%lf\n",ret->unit,unit_no,ret->power,tune_gain,Rf_Config.dac_margin_ax - Rf_Config.dac_margin);
	if(ret->unit >= 2)
	{
		//ac80+80 模式
		tune_gain -= 3;  //AC80+80 功率由两条链路合成， 单条链路功率值减半

		ret->tx_gain_parm.att_code[VSG_ATT_NO3] = get_att_code(unit_no, VSG_ATT_NO3, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on);
		if(ret->unit == 2 || ret->unit == 5)
		{
			ret->tx_gain_parm.att_code[VSG_ATT_NO4] = get_att_code(unit_no, VSG_ATT_NO4, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on);
			share_att4_code = ret->tx_gain_parm.att_code[VSG_ATT_NO4];
		}
		else
		{
			//从机链路，公共att4使用主机配置的attcode,使之相等
			if(ret->unit == 3)
			{
				//att使用信号经过的链路，单元2做从机，单元1做主机，单元2经过的信号会到单元1公共的att4再经单元1的输出端口
				share_unit = 0;
			}
			else
			{
				share_unit = 1;
			}
			get_new_tune_gain(share_unit, VSG_ATT_NO4, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on, share_att4_code);
			printf("congji tune_gain=%lf,share_unit=%d\n",tune_gain,share_unit);
			ret->tx_gain_parm.att_code[VSG_ATT_NO4] = share_att4_code;
		}
	}
	else
	{
		ret->tx_gain_parm.att_code[VSG_ATT_NO4] = get_att_code(unit_no, VSG_ATT_NO4, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on);
		ret->tx_gain_parm.att_code[VSG_ATT_NO3] = get_att_code(unit_no, VSG_ATT_NO3, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on);
	}

	ret->tx_gain_parm.att_code[VSG_ATT_NO2] = get_att_code(unit_no, VSG_ATT_NO2, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on);
	ret->tx_gain_parm.att_code[VSG_ATT_NO1] = get_att_code(unit_no, VSG_ATT_NO1, freq_index, &tune_gain, ret->tx_gain_parm.is_pa_on);
	ret->tx_gain_parm.dac_gain = tune_gain;
	ret->tx_gain_parm.actual_mpl = ret->power - tune_gain;
	printf("is_pa_on=%d,att1=%d,att2=%d,att3=%d,att4=%d,tune_gain=%lf\n",ret->tx_gain_parm.is_pa_on,ret->tx_gain_parm.att_code[VSG_ATT_NO1],ret->tx_gain_parm.att_code[VSG_ATT_NO2],ret->tx_gain_parm.att_code[VSG_ATT_NO3],ret->tx_gain_parm.att_code[VSG_ATT_NO4],tune_gain);
	return 0;
}

void Att_vsg::set_tx_const_gain(double const_gain)
{
	m_const_gain = const_gain;
}

int Att_vsg::get_att_pa_on_calibration_data(char *buf)
{
	int len;
	int unit = 0;
	int ret = ERR_RET_OK;
	char att_file_name[64] = {0};
	for(unit = 0; unit < 2; unit++)
	{
		sprintf(att_file_name,"VSG/vsg_%d_att_1.csv",unit+1);
		len = p_csv_manager->csv_file_get(att_file_name, buf);
		ret = get_att_value_from_buf(buf, len, VSG_ATT_NO1, unit);

		sprintf(att_file_name,"VSG/vsg_%d_att_2.csv",unit+1);
		len = p_csv_manager->csv_file_get(att_file_name, buf);
		ret = get_att_value_from_buf(buf, len, VSG_ATT_NO2, unit);

		sprintf(att_file_name,"VSG/vsg_%d_att_3.csv",unit+1);
		len = p_csv_manager->csv_file_get(att_file_name, buf);
		ret = get_att_value_from_buf(buf, len, VSG_ATT_NO3, unit);

		sprintf(att_file_name,"VSG/vsg_%d_att_4.csv",unit+1);
		len = p_csv_manager->csv_file_get(att_file_name, buf);
		ret = get_att_value_from_buf(buf, len, VSG_ATT_NO4, unit);

		fill_cal_data(unit);
	}
	return ret;
}

int Att_vsg::get_att_off_calibration_data()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		memcpy(&m_cal_data[i].att_pa_off, &m_cal_data[i].att, sizeof(m_cal_data->att));
	}
	return 0;
}

int Att_vsg::get_att_value_from_buf(char *file_buf, int len, int device, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int j;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TX_ATT1_CSV_LOST + device;
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
		for (j=0; j<ATT_CODE_MAX; j++)
		{
			/* 因为第一列是频率，所以j需要加1 */
			ret = App_lib::get_csv_float_item(line, j + 1, &m_cal_data[freq-FREQ_BASE].att[unit].att_value[device][j]);
			err_break(ret);
		}
	}
	return ret;
}

int Att_vsg::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Att_vsg::fill_cal_data(int unit)
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
			int device;
			for(device = 0; device < TX_ATT_COUNT; device++)
			{
				for (k=0; k<ATT_CODE_MAX; k++)
				{
					/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
					m_cal_data[j].att[unit].att_value[device][k] =
							(double)(m_cal_data[freq_index1].att[unit].att_value[device][k]) +
							(double)(m_cal_data[freq_index2].att[unit].att_value[device][k] - m_cal_data[freq_index1].att[unit].att_value[device][k]) /
							(double)(freq_index2 - freq_index1) *
							(double)(j - freq_index1);
				}
			}
		}
	}
	return 0;
}

int Att_vsg::get_att_code(int unit, int device, int freq, double *gain, int is_pa_on)
{
	double delta_gain = 0;
	double default_gain = 0;
	int code = 0;
	int default_code = 0;
	int i;
	int max_att_code = 51;

	if (device == VSG_ATT_NO1)
	{
		max_att_code = ATT_CODE_MAX;
	}

	if (is_pa_on)
	{
		if(Rf_set::is_5g(freq+FREQ_BASE))
		{
			default_code = Rf_Config.tx_att_5g_pa_on_code[device];
		}
		else
		{
			default_code = Rf_Config.tx_att_2g4_pa_on_code[device];
		}
	}
	else
	{
		if(Rf_set::is_5g(freq+FREQ_BASE))
		{
			default_code = Rf_Config.tx_att_5g_pa_off_code[device];
		}
		else
		{
			default_code = Rf_Config.tx_att_2g4_pa_off_code[device];
		}
	}

	if (is_pa_on)
	{
		default_gain = m_cal_data[freq].att[unit].att_value[device][default_code];
		delta_gain = fabs(m_cal_data[freq].att[unit].att_value[device][0] - default_gain - *gain);
		for(i=0; i<max_att_code; i++)
		{
			if (fabs(m_cal_data[freq].att[unit].att_value[device][i] - default_gain - *gain) < delta_gain)
			{
				code = i;
				delta_gain = fabs(m_cal_data[freq].att[unit].att_value[device][i] - default_gain - *gain);
			}
		}
		*gain -= m_cal_data[freq].att[unit].att_value[device][code] - default_gain;
	}
	else
	{
		default_gain = m_cal_data[freq].att_pa_off[unit].att_value[device][default_code];
		delta_gain = fabs(m_cal_data[freq].att_pa_off[unit].att_value[device][0] - default_gain - *gain);
		for(i=0; i<max_att_code; i++)
		{
			if (fabs(m_cal_data[freq].att_pa_off[unit].att_value[device][i] - default_gain - *gain) < delta_gain)
			{
				code = i;
				delta_gain = fabs(m_cal_data[freq].att_pa_off[unit].att_value[device][i] - default_gain - *gain);
			}
		}
		*gain -= m_cal_data[freq].att_pa_off[unit].att_value[device][code] - default_gain;
	}
	return code;
}

int Att_vsg::get_new_tune_gain(int unit, int device, int freq, double *gain, int is_pa_on, int att_code)
{
	double default_gain = 0;
	int default_code = 0;

	if (is_pa_on)
	{
		if(Rf_set::is_5g(freq+FREQ_BASE))
		{
			default_code = Rf_Config.tx_att_5g_pa_on_code[device];
		}
		else
		{
			default_code = Rf_Config.tx_att_2g4_pa_on_code[device];
		}
	}
	else
	{
		if(Rf_set::is_5g(freq+FREQ_BASE))
		{
			default_code = Rf_Config.tx_att_5g_pa_off_code[device];
		}
		else
		{
			default_code = Rf_Config.tx_att_2g4_pa_off_code[device];
		}
	}

	if (is_pa_on)
	{
		default_gain = m_cal_data[freq].att[unit].att_value[device][default_code];
		*gain -= m_cal_data[freq].att[unit].att_value[device][att_code] - default_gain;
	}
	else
	{
		default_gain = m_cal_data[freq].att_pa_off[unit].att_value[device][default_code];
		*gain -= m_cal_data[freq].att_pa_off[unit].att_value[device][att_code] - default_gain;
	}
	return att_code;
}
