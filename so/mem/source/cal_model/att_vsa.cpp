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
#include "../../include/cal_model/att_vsa.h"
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/hal/cal_data.h"
#include "../../include/hal/rf_set.h"
#include "../../include/config/config.h"

#define MAX_ATT_CODE 50

Att_vsa::Att_vsa(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_rx_att *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_rx_att));
	init_freq();
	m_const_gain = 0.0;
	share_att1_code = 0;
	share_att2_code = 0;
}

Att_vsa::~Att_vsa()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

int Att_vsa::wt_calibration_initial(char *buf)
{
	int ret = ERR_RET_OK;
	ret = get_att_pa_on_calibration_data(buf);
	return ret;
}

void Att_vsa::calibration_data_reset()
{
	memset(m_cal_data, 0, MAX_CHANNEL_COUNT * sizeof(T_rx_att));
	init_freq();
	m_const_gain = 0.0;
	share_att1_code = 0;
	share_att2_code = 0;
}

double Att_vsa::get_calibration_data(char *ref_buf)
{
	double tune_gain = 0;	/* 需要的可调增益 */
	double expected_tune_gain = 0;	/* 需要的可调增益 */
	Rx_Parm *ret = (Rx_Parm *)ref_buf;
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	expected_tune_gain = Rf_Config.adc_expected_power - ret->ref_power - m_const_gain;

	int unit_no = get_unit_no(ret->unit);
	if(ret->unit == 2 || ret->unit == 3)
	{
		unit_no = 0;
	}
	else if(ret->unit == 4 || ret->unit == 5)
	{
		unit_no = 1;
	}
	if(ret->unit >= 2)
	{
		expected_tune_gain -= 3;
	}
	tune_gain = expected_tune_gain;
	printf("m_const_gain=%lf,ret->ref_power=%lf,Rf_Config.adc_expected_power=%lf,expected_tune_gain=%lf\n",m_const_gain,ret->ref_power,Rf_Config.adc_expected_power,expected_tune_gain);
	ret->rx_gain_parm.actual_mpl = ret->ref_power;
	if(ret->unit >= 2)
	{
		if(ret->unit == 2 || ret->unit == 5)
		{
			ret->rx_gain_parm.att_code[VSA_ATT_NO1] = get_att_code(unit_no, VSA_ATT_NO1, freq_index, &tune_gain);
			ret->rx_gain_parm.att_code[VSA_ATT_NO2] = get_att_code(Rf_Config.rx_subband6 == 0 ? unit_no : get_unit_no(ret->unit),
			                                                       VSA_ATT_NO2, freq_index, &tune_gain);
			share_att1_code = ret->rx_gain_parm.att_code[VSG_ATT_NO1];
			share_att2_code = ret->rx_gain_parm.att_code[VSG_ATT_NO2];
		}
		else
		{
			get_new_tune_gain(unit_no, VSG_ATT_NO1, freq_index, &tune_gain, share_att1_code);
			if (Rf_Config.rx_subband6 == 0)
			{
				get_new_tune_gain(unit_no, VSG_ATT_NO2, freq_index, &tune_gain, share_att2_code);
			}
			else
			{
				ret->rx_gain_parm.att_code[VSA_ATT_NO2] = get_att_code(get_unit_no(ret->unit), VSA_ATT_NO2, freq_index, &tune_gain);
			}

			printf("congji tune_gain=%lf,share_unit=%d\n",tune_gain, unit_no);
			ret->rx_gain_parm.att_code[VSG_ATT_NO1] = share_att1_code;
		}
	}
	else
	{
		ret->rx_gain_parm.att_code[VSA_ATT_NO1] = get_att_code(unit_no, VSA_ATT_NO1, freq_index, &tune_gain);
		ret->rx_gain_parm.att_code[VSA_ATT_NO2] = get_att_code(unit_no, VSA_ATT_NO2, freq_index, &tune_gain);
	}

	ret->rx_gain_parm.final_gain = m_const_gain + expected_tune_gain - tune_gain;
	ret->rx_gain_parm.actual_mpl = Rf_Config.adc_expected_power - ret->rx_gain_parm.final_gain;
	if(ret->unit >= 2)
	{
		ret->rx_gain_parm.actual_mpl -= 3;
	}

	printf("vsa unit_no=%d,unit=%d,att1=%d,att2=%d,final_gain=%lf,actual_mpl=%lf,tune_gain=%lf\n",unit_no,ret->unit,ret->rx_gain_parm.att_code[VSA_ATT_NO1],ret->rx_gain_parm.att_code[VSA_ATT_NO2],ret->rx_gain_parm.final_gain,ret->rx_gain_parm.actual_mpl,tune_gain);
	return 0;
}

void Att_vsa::get_cal_mode_data(char *ref_buf)
{
	double tune_gain = 0;	/* 需要的可调增益 */
	double expected_tune_gain = 0;	/* 需要的可调增益 */
	Rx_Parm *ret = (Rx_Parm *)ref_buf;
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	expected_tune_gain = Rf_Config.adc_expected_power - ret->ref_power - m_const_gain;
	tune_gain = expected_tune_gain;
	int unit_no = get_unit_no(ret->unit);
	if(ret->unit == 2 || ret->unit == 3)
	{
		unit_no = 0;
	}
	else if(ret->unit == 4 || ret->unit == 5)
	{
		unit_no = 1;
	}

	printf("calibration_flat m_const_gain=%lf,ret->ref_power=%lf,Rf_Config.adc_expected_power=%lf,expected_tune_gain=%lf\n",m_const_gain,ret->ref_power,Rf_Config.adc_expected_power,expected_tune_gain);
	ret->rx_gain_parm.actual_mpl = ret->ref_power;


	if(Rf_set::is_5g(freq_index+FREQ_BASE))
	{
		ret->rx_gain_parm.att_code[VSA_ATT_NO1] = Rf_Config.rx_att_5g_code[VSA_ATT_NO1];
		ret->rx_gain_parm.att_code[VSA_ATT_NO2] = Rf_Config.rx_att_5g_code[VSA_ATT_NO2];
	}
	else
	{
		ret->rx_gain_parm.att_code[VSA_ATT_NO1] = Rf_Config.rx_att_2g4_code[VSA_ATT_NO1];
		ret->rx_gain_parm.att_code[VSA_ATT_NO2] = Rf_Config.rx_att_2g4_code[VSA_ATT_NO2];
	}


	get_new_tune_gain(unit_no, VSG_ATT_NO1, freq_index, &tune_gain, ret->rx_gain_parm.att_code[VSA_ATT_NO1]);
	get_new_tune_gain(unit_no, VSG_ATT_NO2, freq_index, &tune_gain, ret->rx_gain_parm.att_code[VSA_ATT_NO2]);

	ret->rx_gain_parm.final_gain = m_const_gain + expected_tune_gain - tune_gain;
	ret->rx_gain_parm.actual_mpl = Rf_Config.adc_expected_power - ret->rx_gain_parm.final_gain;

	printf("calibration_flat vsa unit_no=%d,unit=%d,att1=%d,att2=%d,final_gain=%lf,actual_mpl=%lf,tune_gain=%lf\n",unit_no,ret->unit,ret->rx_gain_parm.att_code[VSA_ATT_NO1],ret->rx_gain_parm.att_code[VSA_ATT_NO2],ret->rx_gain_parm.final_gain,ret->rx_gain_parm.actual_mpl,tune_gain);
}

void Att_vsa::set_rx_const_gain(double const_gain)
{
	m_const_gain = const_gain;
}

int Att_vsa::get_att_pa_on_calibration_data(char *buf)
{
	int len;
	int unit = 0;
	int ret = ERR_RET_OK;
	char att_file_name[64] = {0};
	for(unit = 0; unit < 2; unit++)
	{
		sprintf(att_file_name, "VSA/vsa_%d_att_1.csv", unit+1);
		len = p_csv_manager->csv_file_get(att_file_name, buf);
		ret = get_att_value_from_buf(buf, len, VSA_ATT_NO1, unit);

		sprintf(att_file_name, "VSA/vsa_%d_att_2.csv", unit+1);
		len = p_csv_manager->csv_file_get(att_file_name, buf);
		ret = get_att_value_from_buf(buf, len, VSA_ATT_NO2, unit);

		fill_cal_data(unit);
	}
	return ret;
}

int Att_vsa::get_att_value_from_buf(char *file_buf, int len, int device, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int j;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_RX_ATT1_CSV_LOST + device;
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

int Att_vsa::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_cal_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Att_vsa::fill_cal_data(int unit)
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
			for(device = 0; device < RX_ATT_COUNT; device++)
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

//*****************************************************************************
// 函数: get_att_code()
// 功能：计算该att需衰减对应功率的最优att code值
// 参数[IN] : unit :  单元编号
//            device: att器件编号
//            freq:   频点索引
//            *gain:  tune_gain 期望衰减的增益
// 返回值: code
//*****************************************************************************
int Att_vsa::get_att_code(int unit, int device, int freq, double *gain)
{
	double delta_gain = 0;
	double default_gain = 0;
	int code = 0;
	int default_code = 0;
	int i;

	if(Rf_set::is_5g(freq+FREQ_BASE))
	{
		default_code = Rf_Config.rx_att_5g_code[device];
	}
	else
	{
		default_code = Rf_Config.rx_att_2g4_code[device];
	}

	default_gain = m_cal_data[freq].att[unit].att_value[device][default_code];
	delta_gain = fabs(m_cal_data[freq].att[unit].att_value[device][0] - default_gain - *gain);
	for(i=0; i<= MAX_ATT_CODE; i++)
	{
		if (fabs(m_cal_data[freq].att[unit].att_value[device][i] - default_gain - *gain) < delta_gain)
		{
			code = i;
			delta_gain = fabs(m_cal_data[freq].att[unit].att_value[device][i] - default_gain - *gain);
		}
	}

	*gain -= m_cal_data[freq].att[unit].att_value[device][code] - default_gain;

	return code;
}

int Att_vsa::get_new_tune_gain(int unit, int device, int freq, double *gain, int att_code)
{
	double default_gain = 0;
	int default_code = 0;

	if(Rf_set::is_5g(freq+FREQ_BASE))
	{
		default_code = Rf_Config.rx_att_5g_code[device];
	}
	else
	{
		default_code = Rf_Config.rx_att_2g4_code[device];
	}

	default_gain = m_cal_data[freq].att[unit].att_value[device][default_code];

	*gain -= m_cal_data[freq].att[unit].att_value[device][att_code] - default_gain;
	return att_code;
}
