/*
 * tx_const_gain_response.cpp
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */
#include <string.h>
#include<stdio.h>
#include <stdlib.h>			//标准函数库定义
#include "../../include/cal_model/tx_response.h"
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/hal/rf_set.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/hal/cal_data.h"
#include "../../include/hal/rf_set.h"
#include "../../include/config/config.h"

Tx_response::Tx_response(int csv_type) :Cal_model(csv_type)
{
	m_cal_data = (T_tx_response_data *)App_lib::app_malloc(FREQ_NUM * sizeof(T_tx_response_data));
	init_freq();
	m_tx_gain = 0;
	data_flag = 0;
	m_sample_freq = 240000000.0;
}

Tx_response::~Tx_response()
{
	if(m_cal_data != NULL)
	{
	    delete m_cal_data;
	    m_cal_data = NULL;
	}
}

void Tx_response::calibration_data_reset()
{
	memset(m_cal_data, 0, FREQ_NUM * sizeof(T_tx_response_data));
	init_freq();
	m_tx_gain = 0;
	data_flag = 0;
	m_sample_freq = 240000000.0;
}

int Tx_response::wt_calibration_initial(char *buf)
{
	int ret = ERR_RET_OK;
	do
	{
		ret = get_tx_rf_response_data(buf);
		err_break(ret);
		ret = get_tx_bb_response_data(buf);
	}while(0);

	return ret;
}

double Tx_response::get_calibration_data(char *ref_buf)
{
	Tx_Parm *ret = (Tx_Parm *)ref_buf;
	int	freq = (u32)((ret->freq + MHz/2)/MHz);
	int rf_port_index = ret->rf_port;
	if(rf_port_index <= WT_SB_PORT_OFF)
	{
		rf_port_index = WT_SB_PORT_OFF;
	}
	else
	{
		rf_port_index -= 1;
	}
	tx_spectral_flatness_compensate(ret->unit, freq, rf_port_index, ret->tx_gain_parm.is_pa_on, &ret->tx_spec_flat_comp_parm);
	return m_tx_gain;
}

double Tx_response::get_tx_const_gain()
{
	return m_tx_gain;
}

int Tx_response::get_tx_rf_response_data(char *file_buf)
{
	int len;
	int ret= ERR_RET_OK;
	int i = 0;
	char att_file_name[64] = {0};
	for(i = 0; i < PORT_MAX; i++)
	{
		sprintf(att_file_name,"VSG/vsg_rf_%d_2G4_freq_response.csv",i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i);

		sprintf(att_file_name,"VSG/vsg_rf_%d_freq_response.csv",i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i);
	}
	return ret;
}

int Tx_response::get_tx_bb_response_data(char *file_buf)
{
	int len;
	int unit = 0;
	int ret= ERR_RET_OK;

	len = p_csv_manager->csv_file_get("VSG/vsg_1_bb_2G4_freq_response.csv", file_buf);
	ret= get_tx_bb_response_from_buf(file_buf, len, 0);

	len = p_csv_manager->csv_file_get("VSG/vsg_2_bb_2G4_freq_response.csv", file_buf);
	ret= get_tx_bb_response_from_buf(file_buf, len, 1);

	len = p_csv_manager->csv_file_get("VSG/vsg_1_bb_freq_response.csv", file_buf);
	ret= get_tx_bb_response_from_buf(file_buf, len, unit);
	unit++;

	len = p_csv_manager->csv_file_get("VSG/vsg_2_bb_freq_response.csv", file_buf);
	ret= get_tx_bb_response_from_buf(file_buf, len, unit);
	return ret;
}

int Tx_response::get_tx_rf_response_from_buf(char *file_buf, int len, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int freq_index = 0;
	int i = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TX_RF_RESPONSE1_CSV_LOST + port;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		freq_index = FREQ_INDEX(freq);

		if (freq <= RESPONSE_2G_CHANNEL_END)
		{
			ret = App_lib::get_csv_float_item(line, 1, &m_cal_data[freq_index].tx_rf_gain.rf_gain_pa[port][0]);
			err_continue(ret);
			ret = App_lib::get_csv_float_item(line, 2, &m_cal_data[freq_index].tx_rf_gain.rf_gain_pa_off[port][0]);
			err_continue(ret);
			continue;
		}

		i = 1;
		for (int j = 0; j < 5; j++)
		{
			ret = App_lib::get_csv_float_item(line, i + j, &m_cal_data[freq_index].tx_rf_gain.rf_gain_pa[port][j]);
			err_break(ret);
		}

		i += 5;
		err_continue(ret);

		for (int j = 0; j < 5; j++)
		{
			ret = App_lib::get_csv_float_item(line, i + j, &m_cal_data[freq_index].tx_rf_gain.rf_gain_pa_off[port][j]);
			err_break(ret);
		}

		i += 5;
		err_continue(ret);

		for (int j = 0; j < 5; j++)
		{
			ret = App_lib::get_csv_float_item(line, i + j, &m_cal_data[freq_index].tx_rf_gain.ac80_host_pa[port][j]);
			err_break(ret);
		}

		i += 5;
		err_continue(ret);

		for (int j = 0; j < 5; j++)
		{
			ret = App_lib::get_csv_float_item(line, i + j, &m_cal_data[freq_index].tx_rf_gain.ac80_host_pa_off[port][j]);
			err_break(ret);
		}

		i += 5;
		err_continue(ret);

		for (int j = 0; j < 5; j++)
		{
			ret = App_lib::get_csv_float_item(line, i + j, &m_cal_data[freq_index].tx_rf_gain.ac80_slave_pa[port][j]);
			err_break(ret);
		}

		i += 5;
		err_continue(ret);

		for (int j = 0; j < 5; j++)
		{
			ret = App_lib::get_csv_float_item(line, i + j, &m_cal_data[freq_index].tx_rf_gain.ac80_slave_pa_off[port][j]);
			err_break(ret);
		}

		i += 5;
		err_continue(ret);

		data_flag = 1;
	}
	return 0;
}

int Tx_response::get_tx_bb_response_from_buf(char *file_buf, int len, int unit)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int freq_index;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TX_BB_RESPONSE1_CSV_LOST + unit;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		freq_index = FREQ_INDEX(freq);
		ret = App_lib::get_csv_float_item(line, 1, &m_cal_data[freq_index].tx_bb_gain.bb_gain[unit][0]);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 2, &m_cal_data[freq_index].tx_bb_gain.bb_gain[unit][1]);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 3, &m_cal_data[freq_index].tx_bb_gain.bb_gain[unit][2]);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 4, &m_cal_data[freq_index].tx_bb_gain.bb_gain[unit][3]);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 5, &m_cal_data[freq_index].tx_bb_gain.bb_gain[unit][4]);
		err_continue(ret);
	}
	return ret;
}

int Tx_response::init_freq()
{
	int i = 0;

	for (i = RESPONSE_2G_CHANNEL_BASE; i <= RESPONSE_2G_CHANNEL_END; i++)
	{
		m_cal_data[i - RESPONSE_2G_CHANNEL_BASE].freq = i;
	}

	for(i = RESPONSE_CHANNEL_BASE; i<= RESPONSE_CHANNEL_END; i++)
	{
		m_cal_data[i - RESPONSE_CHANNEL_BASE + FREQ_2G_NUM].freq = i;
	}


	return 0;
}

void Tx_response::set_tx_sample_freq(double sample_freq)
{
	m_sample_freq = sample_freq;
}

int Tx_response::tx_spectral_flatness_compensate(int unit, int CF, int port, int is_pa_on, Tx_Spec_Flat_Comp_Parm *tx_flat_comp_parm)
{
	int unit_no = get_unit_no(unit);
	double bb_power[769] = {0};
	double rf_power[769] = {0};
	int i = 0;
	int if_5g = 0;
	int if_5g_index = 0;
	int base_bb_freq = 0;
	int freq_index = 0;

	if (CF <= RESPONSE_2G_CHANNEL_END)
	{
		freq_index = FREQ_INDEX((2400 + 2500) / 2 - TX_SPAN_OLD/2);
		for(i = 0; i < TX_SPAN_OLD; i++)
		{
			bb_power[i] = m_cal_data[freq_index + i].tx_bb_gain.bb_gain[unit_no][0];
		}
	}
	else
	{
		if_5g_index = Rf_set::get_tx_if_5g(CF, &if_5g);
		printf("tx_spectral_flatness_compensate,CF=%d,unit=%d,unit_no=%d,if_5g_index=%d,if_5g=%d,is_pa_on=%d,port=%d\n",CF,unit,unit_no,if_5g_index,if_5g,is_pa_on,port);
		if(if_5g_index == 0)
		{
			base_bb_freq = ((4900 + Rf_set::get_tx_if_5g_by_index(if_5g_index)) /2) - 120;
			printf("1base_bb_freq=%d\n",base_bb_freq);
			freq_index = FREQ_INDEX(base_bb_freq);
			for(i = 0; i < TX_SPAN_OLD; i++)
			{
				bb_power[i] = m_cal_data[freq_index+i].tx_bb_gain.bb_gain[unit_no][if_5g_index];
			}
		}
		else
		{
			base_bb_freq = ((Rf_set::get_tx_if_5g_by_index(if_5g_index - 1) + Rf_set::get_tx_if_5g_by_index(if_5g_index)) /2) - 120;
			printf("2base_bb_freq=%d\n",base_bb_freq);
			freq_index = FREQ_INDEX(base_bb_freq);
			for(i = 0; i < TX_SPAN_OLD; i++)
			{
				bb_power[i] = m_cal_data[freq_index+i].tx_bb_gain.bb_gain[unit_no][if_5g_index];
			}
		}
	}

	for(i = 0; i < 10; i++)
	{
		// 填充前后10个数据      射频功率左右两边10个点，使用邻近一个点的数据
		//bb_power[i] = bb_power[10];
		//bb_power[231+i] = bb_power[230];

		/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
		bb_power[i] =(bb_power[10]) + ((bb_power[20] - bb_power[10]) / (double)(20 - 10)) * (double)(i - 10);
		bb_power[231+i] = bb_power[230] + (bb_power[220] - bb_power[230]) / (double)(220 - 230) * (double)(231 + i - 230);
	}

	int base_freq_index = CF - TX_SPAN_OLD/2 ;
	freq_index = FREQ_INDEX(base_freq_index);

	if(unit >= 2 && data_flag)
	{
		printf("tx_response data_flag=%d\n",data_flag);
		if(unit == 2 || unit == 5)
		{
			if (is_pa_on)
			{
				for (i = 0; i < TX_SPAN_OLD; i++)
				{
					rf_power[i] = m_cal_data[freq_index+i].tx_rf_gain.ac80_host_pa[port][if_5g_index];
				}
			}
			else
			{
				for (i = 0; i < TX_SPAN_OLD; i++)
				{
					rf_power[i] = m_cal_data[freq_index+i].tx_rf_gain.ac80_host_pa_off[port][if_5g_index];
				}
			}
		}
		else
		{
			if (is_pa_on)
			{
				for (i = 0; i < TX_SPAN_OLD; i++)
				{
					rf_power[i] = m_cal_data[freq_index+i].tx_rf_gain.ac80_slave_pa[port][if_5g_index];
				}
			}
			else
			{
				for (i = 0; i < TX_SPAN_OLD; i++)
				{
					rf_power[i] = m_cal_data[freq_index+i].tx_rf_gain.ac80_slave_pa_off[port][if_5g_index];
				}
			}
		}
	}
	else
	{
		printf("putongshuju,if_5g_index=%d,freq_index=%d\n",if_5g_index,freq_index);
		if (is_pa_on)
		{
			for (i = 0; i < TX_SPAN_OLD; i++)
			{
				rf_power[i] = m_cal_data[freq_index+i].tx_rf_gain.rf_gain_pa[port][if_5g_index];
			}
		}
		else
		{
			for (i = 0; i < TX_SPAN_OLD; i++)
			{
				rf_power[i] = m_cal_data[freq_index+i].tx_rf_gain.rf_gain_pa_off[port][if_5g_index];
			}
		}
	}

	for(i = 0; i < 10; i++)
	{
		// 填充前后10个数据      射频功率左右两边10个点，使用邻近一个点的数据
		//rf_power[i] = rf_power[10];
		//rf_power[231+i] = rf_power[230];

		rf_power[i] =(rf_power[10]) + ((rf_power[20] - rf_power[10]) / (double)(20 - 10)) * (double)(i - 10);
		rf_power[231+i] = rf_power[230] + (rf_power[220] - rf_power[230]) / (double)(220 - 230) * (double)(231 + i - 230);
	}

//	for (i = 0; i < TX_SPAN_OLD; i++)
//	{
//		tx_flat_comp_parm->bb_comp_gain[i] = bb_power[i];
//		tx_flat_comp_parm->rf_comp_gain[i] = rf_power[i];
//	}

	int span = (int)m_sample_freq / MHz;
	int start_index = TX_SPAN_OLD / 2 - span / 2;
	memcpy(tx_flat_comp_parm->bb_comp_gain, &bb_power[start_index], (span + 1) * sizeof(double));
	memcpy(tx_flat_comp_parm->rf_comp_gain, &rf_power[start_index], (span + 1) * sizeof(double));
	tx_flat_comp_parm->bb_comp_len = span+1;
	tx_flat_comp_parm->rf_comp_len = span+1;

	return 0;
}

//===============================================================================

Tx_response_v2::Tx_response_v2(int csv_type) : Cal_model(csv_type)
{
	for (int i = 0; i < PORT_MAX; i++)
	{
		m_cal_data[i] = (tx_response_item *)App_lib::app_malloc(FREQ_NUM * sizeof(tx_response_item));
	}

	m_tx_gain = 0;
	data_flag = 0;
	m_sample_freq = 240000000.0;
}

Tx_response_v2::~Tx_response_v2()
{
	for (int i = 0; i < PORT_MAX; i++)
	{
		delete m_cal_data[i];
	}
}

void Tx_response_v2::calibration_data_reset()
{
	for (int i = 0; i < PORT_MAX; i++)
	{
		memset(m_cal_data[i], 0, FREQ_NUM * sizeof(tx_response_item));
	}

	m_tx_gain = 0;
	data_flag = 0;
	m_sample_freq = 240000000.0;
}

int Tx_response_v2::wt_calibration_initial(char *buf)
{
	int ret = ERR_RET_OK;
	do
	{
		ret = get_tx_rf_response_data(buf);
		err_break(ret);
	}while(0);

	return ret;
}

int Tx_response_v2::get_tx_rf_response_data(char *file_buf)
{
	int len;
	int ret= ERR_RET_OK;
	int i = 0;
	char att_file_name[64] = {0};
	for(i = 0; i < PORT_MAX; i++)
	{
		sprintf(att_file_name, "VSG/vsg_rf_%d_PaOn_flatness.csv", i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i, 0);

		sprintf(att_file_name, "VSG/vsg_rf_%d_PaOff_flatness.csv", i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i, 1);

		sprintf(att_file_name, "VSG/vsg_rf_%d_ac8080HostPaOn_flatness.csv", i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i, 2);

		sprintf(att_file_name, "VSG/vsg_rf_%d_ac8080HostPaOff_flatness.csv", i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i, 3);

		sprintf(att_file_name, "VSG/vsg_rf_%d_ac8080SlavePaOn_flatness.csv", i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i, 4);

		sprintf(att_file_name, "VSG/vsg_rf_%d_ac8080SlavePaOff_flatness.csv", i+1);
		len = p_csv_manager->csv_file_get(att_file_name, file_buf);
		ret= get_tx_rf_response_from_buf(file_buf, len, i, 5);

		data_flag = 1;
	}

	ret= ERR_RET_OK;

	return ret;
}

int Tx_response_v2::get_tx_rf_response_from_buf(char *file_buf, int len, int port, int type)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int freq_index = 0;
	int i = 0;
	int offset = type * TX_SPAN;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		printf("get_tx_rf_response_from_buf failed : ERR_TX_RF_RESPONSE1_CSV_LOST %d\n", port);
		return ERR_TX_RF_RESPONSE1_CSV_LOST + port;
	}

	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}

		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		freq_index = FREQ_INDEX(freq);

		if(Rf_Config.response_data_flag)
		{
			for (i = 0; i < TX_SPAN; i++)
			{
				ret = App_lib::get_csv_float_item(line, i + 1, m_cal_data[port][freq_index].gain_pa + offset + i);
				err_break(ret);
			}

			err_continue(ret);
		}
		else
		{
			for (i = 0; i < TX_SPAN_OLD - 20; i++)
			{
				ret = App_lib::get_csv_float_item(line, i + 1, m_cal_data[port][freq_index].gain_pa + offset + i + 10);
				err_break(ret);
			}

			err_continue(ret);
			// 填充前后10个数据
			double *rf_power = m_cal_data[port][freq_index].gain_pa + offset;
			for(i = 0; i < 10; i++)
			{
				rf_power[i] =(rf_power[10]) + ((rf_power[20] - rf_power[10]) / (double)(20 - 10)) * (double)(i - 10);
				rf_power[231+i] = rf_power[230] + (rf_power[220] - rf_power[230]) / (double)(220 - 230) * (double)(231 + i - 230);
			}
		}
	}
	return 0;
}

double Tx_response_v2::get_tx_const_gain()
{
	return m_tx_gain;
}

double Tx_response_v2::get_calibration_data(char *ref_buf)
{
	Tx_Parm *ret = (Tx_Parm *)ref_buf;
	int	freq = (u32)((ret->freq + MHz/2)/MHz);
	int rf_port_index = ret->rf_port;
	if(rf_port_index <= WT_SB_PORT_OFF)
	{
		rf_port_index = WT_SB_PORT_OFF;
	}
	else
	{
		rf_port_index -= 1;
	}
	tx_spectral_flatness_compensate(ret->unit, m_sample_freq, freq, rf_port_index, ret->tx_gain_parm.is_pa_on, &ret->tx_spec_flat_comp_parm);
	return m_tx_gain;
}

void Tx_response_v2::set_tx_sample_freq(double sample_freq)
{
	m_sample_freq = sample_freq;
}

int Tx_response_v2::get_2g4_nearest_freq(int CF)
{
	static int cal_freq[10] = {2402, 2412, 2422, 2432, 2442, 2452, 2462, 2472, 2482, 2492};
	int i= 0;
	int deata_freq = 50;
	int ch1 = 0;
	for(i=0; i<10; i++)
	{
		if(abs(CF-cal_freq[i]) < deata_freq)
		{
			deata_freq  = abs(CF-cal_freq[i]);
			ch1 = i;
		}
	}
	return cal_freq[ch1];
}

int Tx_response_v2::tx_spectral_flatness_compensate(int unit, double sample_freq, int CF, int port, int is_pa_on,
                                                    Tx_Spec_Flat_Comp_Parm *tx_flat_comp_parm)
{
	int if_5g = 0;
	int if_5g_index = 0;
	int freq_index = FREQ_INDEX(CF);

	memset(tx_flat_comp_parm->bb_comp_gain, 0, TX_SPECTRUM_SUBCHAN_COUNT * sizeof(double));
	memset(tx_flat_comp_parm->rf_comp_gain, 0, TX_SPECTRUM_SUBCHAN_COUNT * sizeof(double));
	int span = 0;
	int start_index = 0;
	printf("tx sample_freq=%lf,Rf_Config.response_data_flag=%d\n",sample_freq,Rf_Config.response_data_flag);
	if(Rf_Config.response_data_flag)
	{
		span =  (sample_freq/240000000.0) * (TX_SPAN-1);
		start_index = TX_SPAN / 2 - span / 2;
	}
	else
	{
		span = (int)sample_freq / MHz;
		start_index = TX_SPAN_OLD / 2 - span / 2;
	}
	printf("11CF=%d,span=%d,start_index=%d,sample_freq=%lf\n",CF,span,start_index,sample_freq);
	if (CF <= RESPONSE_2G_CHANNEL_END)
	{
		int freq = get_2g4_nearest_freq(CF);
		freq_index = FREQ_INDEX(freq);
		printf("11CF=%d,freq=%d,freq_index=%d\n",CF,freq,freq_index);
		if (is_pa_on)
		{
			memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].gain_pa[start_index], (span + 1) * sizeof(double));
		}
		else
		{
			memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].gain_pa_off[start_index], (span + 1) * sizeof(double));
		}
	}
	else
	{
		if_5g_index = Rf_set::get_tx_if_5g(CF, &if_5g);
		int freq = 0;
		if(CF < RESPONSE_CHANNEL_MID || Rf_Config.response_data_flag == 0)
		{
			freq = App_lib::near_num(CF, 10);
			if (Rf_set::get_tx_if_5g(freq, &if_5g) > if_5g_index)
			{
				freq -= 10;
			}
			else if (Rf_set::get_tx_if_5g(freq, &if_5g) < if_5g_index)
			{
				freq += 10;
			}
		}
		else
		{
			freq = App_lib::near_num(CF, 5);
			if (Rf_set::get_tx_if_5g(freq, &if_5g) > if_5g_index)
			{
				freq -= 5;
			}
			else if (Rf_set::get_tx_if_5g(freq, &if_5g) < if_5g_index)
			{
				freq += 5;
			}
		}

		freq_index = FREQ_INDEX(freq);
		printf("22CF=%d,freq=%d,freq_index=%d\n",CF,freq,freq_index);

		if(unit >= 2 && data_flag)
		{
			if(unit == 2 || unit == 5)
			{
				if (is_pa_on)
				{
					memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].host_gain_pa[start_index], (span + 1) * sizeof(double));
				}
				else
				{
					memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].host_gain_pa_off[start_index], (span + 1) * sizeof(double));
				}
			}
			else
			{
				if (is_pa_on)
				{
					memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].slave_gain_pa[start_index], (span + 1) * sizeof(double));
				}
				else
				{
					memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].slave_gain_pa_off[start_index], (span + 1) * sizeof(double));
				}
			}
		}
		else
		{
			if (is_pa_on)
			{
				memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].gain_pa[start_index], (span + 1) * sizeof(double));
			}
			else
			{
				memcpy(tx_flat_comp_parm->rf_comp_gain, &m_cal_data[port][freq_index].gain_pa_off[start_index], (span + 1) * sizeof(double));
			}
		}
	}
	tx_flat_comp_parm->bb_comp_len = (span + 1);
	tx_flat_comp_parm->rf_comp_len = (span + 1);
//	int i;
//	for(i=0;i<TX_SPECTRUM_SUBCHAN_COUNT;i++)
//	{
//		printf("tx_flat_comp_parm->rf_comp_gain[%d]=%lf\n",i,tx_flat_comp_parm->rf_comp_gain[i]);
//	}


	return 0;
}
