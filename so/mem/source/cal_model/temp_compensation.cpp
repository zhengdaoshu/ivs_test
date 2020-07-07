/*
 * temp_compensation.cpp
 *
 *  Created on: 2016年12月15日
 *      Author: J00054
 */
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include "../../include/cal_model/temp_compensation.h"
#include "../../include/hal/cal_data.h"
#include "../../include/base/cal_error.h"
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/csv_file_manager/csv_disk.h"
#include "../../include/csv_file_manager/csv_factory.h"
#include "../../include/wt-calibration.h"
#include "../../include/config/config.h"

Temp_config::Temp_config(int csv_type) :Cal_model(csv_type)
{
	m_temp_data = (T_temp_config *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(T_temp_config));
	init_freq();
	init_temp_variable();
	m_src_type = src_disk;
	p_csv_manager = Csv_factory::get_csv_manager(m_src_type);
}

Temp_config::~Temp_config()
{
	if(m_temp_data != NULL)
	{
		delete m_temp_data;
	}
}

void Temp_config::calibration_data_reset()
{
	memset(m_temp_data, 0, MAX_CHANNEL_COUNT * sizeof(T_temp_config));
	init_freq();
	init_temp_variable();
}

void Temp_config::init_temp_variable()
{
	int i;
	temp_flag = 0;

	cal_rx_temp[0] = 0.0;
	cal_rx_temp[1] = 0.0;
	cal_tx_temp[0] = 0.0;
	cal_tx_temp[1] = 0.0;
	for(i=0; i<PORT_MAX; i++)
	{
		tx_port_temp[i] = 0.0;
		rx_port_temp[i] = 0.0;
		tx_sw_port_temp[i] = 0.0;
		rx_sw_port_temp[i] = 0.0;
	}
}

int Temp_config::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		m_temp_data[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Temp_config::wt_calibration_initial(char *buf)
{
	int len = 0;
	int ret = 0;
	int i = 0, j = 0;
	char csv_file_name[64] = {0};
	do
	{
		init_temp_variable();
		len = p_csv_manager->csv_file_get("calibration_report.csv", buf);
		ret = get_cal_temp_from_buf(buf, len);
		err_break(ret);
		ret = get_cal_report_version_from_buf(buf, len);
		err_break(ret);
		reset_dac_margin_value();
		check_response_version();

		sprintf(csv_file_name, "temp_band1/temp_tx_rf1.csv");
		len = ((Csv_disk *)p_csv_manager)->get_configuration_data(csv_file_name, buf, CAL_FILE_SIZE);
		if(len <= 0)
		{
			printf("old tempcompensation file\n");
			for(i=0; i<TEMP_BAND_COUNT; i++)
			{
				for(j=0; j<PORT_MAX; j++)
				{
					sprintf(csv_file_name, "temp_band%d/temp_compensation_rf%d.csv", i+1, j+1);
					len = ((Csv_disk *)p_csv_manager)->get_configuration_data(csv_file_name, buf, CAL_FILE_SIZE);
					ret = get_temp_compensation_from_buf(buf, len, i, j);
					err_break(ret);
					fill_cal_data(i, j);
				}
			}

			return ret;
		}

		for(i=0; i<TEMP_BAND_COUNT; i++)
		{
			for(j=0; j<PORT_MAX; j++)
			{
				sprintf(csv_file_name, "temp_band%d/temp_tx_rf%d.csv", i+1, j+1);
				len = ((Csv_disk *)p_csv_manager)->get_configuration_data(csv_file_name, buf, CAL_FILE_SIZE);
				//printf("csv_file_name=%s,len=%d\n",csv_file_name,len);
				ret = get_tx_temp_compensation_from_buf(buf, len, i, j);
				//printf("csv_file_name=%s,len=%d,ret=%d\n",csv_file_name,len,ret);
				err_break(ret);

				if(j < (PORT_MAX-1))
				{
					sprintf(csv_file_name, "temp_band%d/temp_rx_rf%d.csv", i+1, j+1);
					len = ((Csv_disk *)p_csv_manager)->get_configuration_data(csv_file_name, buf, CAL_FILE_SIZE);
					ret = get_rx_temp_compensation_from_buf(buf, len, i, j);
					err_break(ret);
				}

				sprintf(csv_file_name, "temp_band%d/temp_tx_sw_rf%d.csv", i+1, j+1);
				len = ((Csv_disk *)p_csv_manager)->get_configuration_data(csv_file_name, buf, CAL_FILE_SIZE);
				ret = get_tx_sw_temp_compensation_from_buf(buf, len, i, j);
				err_break(ret);

				if(j < (PORT_MAX-1))
				{
					sprintf(csv_file_name, "temp_band%d/temp_rx_sw_rf%d.csv", i+1, j+1);
					len = ((Csv_disk *)p_csv_manager)->get_configuration_data(csv_file_name, buf, CAL_FILE_SIZE);
					ret = get_rx_sw_temp_compensation_from_buf(buf, len, i, j);
					err_break(ret);
				}

				fill_tx_cal_data(i, j);
				fill_rx_cal_data(i, j);

				fill_tx_sw_cal_data(i, j);
				fill_rx_sw_cal_data(i, j);
			}
			err_break(ret);
		}
	}while(0);
//	show_sw_cal_data(0,2);
//	show_cal_data(0,2);
	return ret;
}

int Temp_config::get_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int i = 0;

	if (len <= 0)
	{
		return ERR_TEMP_COMPENSATION_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		for(i=0; i<TEMP_COLOUM; i++)
		{
			ret = App_lib::get_csv_float_item(line, i+1, &m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][i]);
			err_break(ret);
		}

		m_temp_data[freq-FREQ_BASE].tx_flag[temp_band][port] = TRUE;
//		if(freq == 2420)
//		{
//			printf("port=%d,freq=%d,temp_band=%d,port=%d,slop=%lf,1=%lf,2=%lf,3=%lf,8=%lf\n",port,freq,temp_band,port,m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][0],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][1],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][2],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][3],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][8]);
//		}

	}

	return ret;
}

double Temp_config::get_calibration_data(char *ref_buf)
{
	return 0;
}

int Temp_config::get_temp_subband_mode(double rf_temperture)
{
	if(rf_temperture < Rf_Config.temp_subband1)
	{
		return Temp_band1;
	}
	else if(rf_temperture < Rf_Config.temp_subband2)
	{
		return Temp_band2;
	}
	else if(rf_temperture < Rf_Config.temp_subband3)
	{
		return Temp_band3;
	}
	else
	{
		return Temp_band4;
	}
}

int Temp_config::get_temp_conlum_type(int mode, int unit, int is_pa_on)
{
	int ret = siso_tx_pa_on;
	if(mode == CONFIG_TYPE_RX)
	{
		if(unit < 2)
		{
			ret = siso_rx;
		}
		else if(unit == 2 || unit == 5)
		{
			ret = ac80_master_rx;
		}
		else
		{
			ret = ac80_slave_rx;
		}
	}
	else
	{
		if(is_pa_on)
		{
			if(unit < 2)
			{
				ret = siso_tx_pa_on;
			}
			else if(unit == 2 || unit == 5)
			{
				ret = ac80_master_tx_pa_on;
			}
			else
			{
				ret = ac80_slave_tx_pa_on;
			}
		}
		else
		{
			if(unit < 2)
			{
				ret = siso_tx_pa_off;
			}
			else if(unit == 2 || unit == 5)
			{
				ret = ac80_master_tx_pa_off;
			}
			else
			{
				ret = ac80_slave_tx_pa_off;
			}
		}
	}
	return ret;
}

double Temp_config::get_tx_compensation(double tx_gain, char *ref_buf)
{
	if(!Rf_Config.tx_temp_cps)
	{
		printf("Rf_Config.tx_temp_cps=%d\n",Rf_Config.tx_temp_cps);
		return tx_gain;
	}
	double slope = 0;
	double const_gain = tx_gain;
	Tx_Parm *ret = (Tx_Parm *)ref_buf;
	int unit_no = get_unit_no(ret->unit);
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	int temp_subband = get_temp_subband_mode(ret->temperature);
	int colunm_index = get_temp_conlum_type(CONFIG_TYPE_TX, ret->unit, ret->tx_gain_parm.is_pa_on);
	int rf_port_index = ret->rf_port;
	if(rf_port_index <= WT_SB_PORT_OFF)
	{
		rf_port_index = WT_SB_PORT_OFF;
	}
	else
	{
		rf_port_index -= WT_SB_PORT_RF1;
	}
	slope = m_temp_data[freq_index].temp_comp[temp_subband][rf_port_index][colunm_index];
	printf("tx temp_flag=%d,ret->temperature=%lf,ret->sw_temperature=%lf,freq=%d,temp_subband=%d,is_pa_on=%d,rf_port_index=%d,colunm_index=%d,slope_value=%lf\n",temp_flag,ret->temperature,ret->sw_temperature,freq_index+FREQ_BASE,temp_subband,ret->tx_gain_parm.is_pa_on,rf_port_index,colunm_index,slope);
	if(temp_flag == 0)
	{
		const_gain += (ret->temperature - cal_tx_temp[unit_no]) * slope;
	}
	else if(temp_flag == 1)
	{
		const_gain += (ret->temperature - tx_port_temp[rf_port_index]) * slope;
	}
	else
	{
		const_gain += (ret->temperature - tx_port_temp[rf_port_index]) * slope;
		temp_subband = get_temp_subband_mode(ret->sw_temperature);
		slope =m_temp_data[freq_index].sw_temp_comp[temp_subband][rf_port_index][0];
		//printf("tx sw flag=%d,ret->sw_temperature=%lf,freq=%d,temp_subband=%d,is_pa_on=%d,rf_port_index=%d,colunm_index=%d,slope_value=%lf\n",temp_flag,ret->sw_temperature,freq_index+FREQ_BASE,temp_subband,ret->tx_gain_parm.is_pa_on,rf_port_index,colunm_index,slope);
		printf("tx sw temp_subband=%d,slope=%lf,ret->sw_temperature=%lf\n",temp_subband,slope,ret->sw_temperature);
		const_gain += (ret->sw_temperature - tx_sw_port_temp[rf_port_index]) * slope;
	}
	return const_gain;
}

double Temp_config::get_rx_compensation(double rx_gain, char *ref_buf)
{
	if(!Rf_Config.rx_temp_cps)
	{
		return rx_gain;
	}
	double slope = 0;
	double const_gain = rx_gain;
	Rx_Parm *ret = (Rx_Parm *)ref_buf;
	int unit_no = get_unit_no(ret->unit);
	int	freq_index = (u32)((ret->freq + MHz/2)/MHz) - FREQ_BASE;
	int temp_subband = get_temp_subband_mode(ret->temperature);
	int colunm_index = get_temp_conlum_type(CONFIG_TYPE_RX, ret->unit, TRUE);
	int rf_port_index = ret->rf_port;
	if(rf_port_index <= WT_SB_PORT_OFF)
	{
		rf_port_index = WT_SB_PORT_OFF;
	}
	else
	{
		rf_port_index -= WT_SB_PORT_RF1;
	}
	slope = m_temp_data[freq_index].temp_comp[temp_subband][rf_port_index][colunm_index];
	printf("rx temp_flag=%d,ret->temperature=%lf,sw_temperature=%lf,freq=%d,temp_subband=%d,rf_port_index=%d,colunm_index=%d,slope_value=%lf\n",temp_flag,ret->temperature,ret->sw_temperature,freq_index+FREQ_BASE,temp_subband,rf_port_index,colunm_index,slope);
	if(temp_flag == 0)
	{
		const_gain += (ret->temperature - cal_rx_temp[unit_no]) * slope;
	}
	else if(temp_flag == 1)
	{
		const_gain += (ret->temperature - rx_port_temp[rf_port_index]) * slope;
	}
	else
	{
		const_gain += (ret->temperature - rx_port_temp[rf_port_index]) * slope;
		temp_subband = get_temp_subband_mode(ret->sw_temperature);
		slope = m_temp_data[freq_index].sw_temp_comp[temp_subband][rf_port_index][1];
		printf("rx sw slope=%lf,ret->sw_temperature=%lf\n",slope,ret->sw_temperature);
		const_gain += (ret->sw_temperature - rx_sw_port_temp[rf_port_index]) * slope;
	}
	return const_gain;
}

void Temp_config::reset_dac_margin_value()
{
	char buf[32] = {0};
	strcpy(buf, "1.2.4.46");
	if(App_lib::fwVersionCmp(cal_report_ver, buf) < 1 )
	{
		Rf_Config.dac_margin = 10.5;
		printf("reset_dac_margin_value Rf_Config.dac_margin=%lf\n",Rf_Config.dac_margin);
	}
	else
	{
		printf("dac_margin_value Rf_Config.dac_margin=%lf\n",Rf_Config.dac_margin);
	}
}

void Temp_config::check_response_version()
{
	char buf[32] = {0};
	strcpy(buf, "1.3.4.58");
	int ret = App_lib::fwVersionCmp(cal_report_ver, buf);
	if(ret < 0 )
	{
		Rf_Config.response_data_flag = 0;
	}
	else
	{
		Rf_Config.response_data_flag = 1;
	}
	printf("check_response_version,Rf_Config.response_data_flag=%d,ret=%d\n",Rf_Config.response_data_flag,ret);
}

int Temp_config::get_cal_report_version_from_buf(char *file_buf, int len)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	char keyword[64] = {0};
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_CAL_REPORT_LOST;
	}
	memset(cal_report_ver, 0, 32);

	/* 读取配置文件中的值 */
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		ret = App_lib::get_csv_string_item(line, 0, keyword);
		err_continue(ret);
		if (!strcmp(keyword, "//CAL-Version"))
		{
			ret = App_lib::get_csv_string_item(line, 1, cal_report_ver);
			break;
		}
		memset(keyword, 0, 64);
	}
	printf("2222222222cal_report_ver=%s\n",cal_report_ver);
	return ret;
}

int Temp_config::get_cal_temp_from_buf(char *file_buf, int len)
{
	int i;
	char str_name[32] = {0};
	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_CAL_REPORT_LOST;
	}
	get_csv_line_value("CAL-Rx-0-Temperature", file_buf, &(cal_rx_temp[0]));
	get_csv_line_value("CAL-Rx-1-Temperature", file_buf, &(cal_rx_temp[1]));
	get_csv_line_value("CAL-TX-0-Temperature", file_buf, &(cal_tx_temp[0]));
	get_csv_line_value("CAL-TX-1-Temperature", file_buf, &(cal_tx_temp[1]));
	printf("temp_flag=%d,cal_rx_temp=%lf,%lf,cal_tx_temp=%lf,%lf\n",temp_flag,cal_rx_temp[0],cal_rx_temp[1],cal_tx_temp[0],cal_tx_temp[1]);

	for(i=0; i<PORT_MAX; i++)
	{
		sprintf(str_name, "CAL-TX-RF%d-Temperature", i+1);
		get_csv_line_value(str_name, file_buf, &(tx_port_temp[i]));
		if(i < VSA_PORT_MAX)
		{
			sprintf(str_name, "CAL-RX-RF%d-Temperature", i+1);
			get_csv_line_value(str_name, file_buf, &(rx_port_temp[i]));
		}
	}

	double sum_temp = 0.0;
	for(i=0;i<VSA_PORT_MAX;i++)
	{
		sum_temp += tx_port_temp[i];
		//sum_temp += rx_port_temp[i];
	}
	Rf_Config.cal_port_temp_avg = sum_temp/(VSA_PORT_MAX);
	
	if(App_lib::double_compare(tx_port_temp[PORT_MAX-1], 0.0))
	{
		temp_flag = 1;
		printf("temp_flag=%d,tx_port_temp=%lf,%lf,rx_port_temp=%lf,%lf\n",temp_flag,tx_port_temp[0],tx_port_temp[8],rx_port_temp[0],rx_port_temp[7]);
	}

	for(i=0; i<PORT_MAX; i++)
	{
		sprintf(str_name, "CAL-TX-SW-RF%d-Temperature", i+1);
		get_csv_line_value(str_name, file_buf, &(tx_sw_port_temp[i]));
		if(i < VSA_PORT_MAX)
		{
			sprintf(str_name, "CAL-RX-SW-RF%d-Temperature", i+1);
			get_csv_line_value(str_name, file_buf, &(rx_sw_port_temp[i]));
		}
	}
	if(App_lib::double_compare(tx_sw_port_temp[PORT_MAX-1], 0.0))
	{
		temp_flag = 2;
		printf("temp_flag=%d,tx_sw_port_temp=%lf,%lf,rx_sw_port_temp=%lf,%lf,Rf_Config.cal_port_temp_avg=%lf\n",temp_flag,tx_sw_port_temp[0],tx_sw_port_temp[8],rx_sw_port_temp[0],rx_sw_port_temp[7],Rf_Config.cal_port_temp_avg);
	}
	return 0;
}

int Temp_config::get_tx_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int i = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TEMP_COMPENSATION_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		for(i=0; i<TX_TEMP_COLOUM; i++)
		{
			ret = App_lib::get_csv_float_item(line, i+1, &m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][i]);
			err_break(ret);
		}

		m_temp_data[freq-FREQ_BASE].tx_flag[temp_band][port] = TRUE;
//		if(freq == 2450)
//		{
//			printf("port=%d,freq=%d,temp_band=%d,port=%d,slop=%lf,1=%lf,2=%lf,3=%lf,4=%lf,5=%lf\n",port,freq,temp_band,port,m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][0],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][1],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][2],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][3],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][4],m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][5]);
//		}
	}
//	if(temp_band == 0)
//	{
//		printf("m_temp_data[2450-FREQ_BASE].flag[temp_band][%d]=%d\n",port,m_temp_data[2450-FREQ_BASE].tx_flag[temp_band][port]);
//	}

	return ret;
}

int Temp_config::get_rx_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;
	int i = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TEMP_COMPENSATION_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		for(i=0; i<RX_TEMP_COLOUM; i++)
		{
			ret = App_lib::get_csv_float_item(line, i+1, &m_temp_data[freq-FREQ_BASE].temp_comp[temp_band][port][TX_TEMP_COLOUM+i]);
			err_break(ret);
		}

		m_temp_data[freq-FREQ_BASE].rx_flag[temp_band][port] = TRUE;

	}
	return ret;
}

int Temp_config::get_tx_sw_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TEMP_COMPENSATION_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		ret = App_lib::get_csv_float_item(line, 1, &m_temp_data[freq-FREQ_BASE].sw_temp_comp[temp_band][port][0]);
		err_break(ret);

		m_temp_data[freq-FREQ_BASE].sw_tx_flag[temp_band][port] = TRUE;
	}

	return ret;
}

int Temp_config::get_rx_sw_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_TEMP_COMPENSATION_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);

		ret = App_lib::get_csv_float_item(line, 1, &m_temp_data[freq-FREQ_BASE].sw_temp_comp[temp_band][port][1]);
		err_break(ret);

		m_temp_data[freq-FREQ_BASE].sw_rx_flag[temp_band][port] = TRUE;
	}
	return ret;
}

int Temp_config::fill_tx_cal_data(int temp_band, int port)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_temp_data[i].tx_flag[temp_band][port])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].tx_flag[temp_band][port])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].tx_flag[temp_band][port] && j != freq_index1)
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
//		if(temp_band == 0)
//		{
//			printf("freq_index1=%d,freq_index2=%d,m_temp_data[freq_index1].freq=%d,m_temp_data[freq_index2].freq=%d\n",freq_index1,freq_index2,m_temp_data[freq_index1].freq,m_temp_data[freq_index2].freq);
//		}
		for(j = min_index; j < max_index; j++)
		{
			/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
			for(k = 0;  k < TX_TEMP_COLOUM; k++)
			{
				m_temp_data[j].tx_flag[temp_band][port] = TRUE;
				m_temp_data[j].temp_comp[temp_band][port][k] =
						(double)(m_temp_data[freq_index1].temp_comp[temp_band][port][k]) +
						(double)(m_temp_data[freq_index2].temp_comp[temp_band][port][k] - m_temp_data[freq_index1].temp_comp[temp_band][port][k]) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1);
			}
		}
	}
	return 0;
}

int Temp_config::fill_rx_cal_data(int temp_band, int port)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_temp_data[i].rx_flag[temp_band][port])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].rx_flag[temp_band][port])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].rx_flag[temp_band][port] && j != freq_index1)
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
			for(k = 0;  k < RX_TEMP_COLOUM; k++)
			{
				m_temp_data[j].rx_flag[temp_band][port] = TRUE;
				m_temp_data[j].temp_comp[temp_band][port][TX_TEMP_COLOUM+k] =
						(double)(m_temp_data[freq_index1].temp_comp[temp_band][port][TX_TEMP_COLOUM+k]) +
						(double)(m_temp_data[freq_index2].temp_comp[temp_band][port][TX_TEMP_COLOUM+k] - m_temp_data[freq_index1].temp_comp[temp_band][port][TX_TEMP_COLOUM+k]) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1);
			}
		}
	}
	return 0;
}

int Temp_config::fill_tx_sw_cal_data(int temp_band, int port)
{
	int i = 0;
	int j = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_temp_data[i].sw_tx_flag[temp_band][port])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].sw_tx_flag[temp_band][port])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].sw_tx_flag[temp_band][port] && j != freq_index1)
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
//		if(temp_band == 0)
//		{
//			printf("tx_swfreq_index1=%d,freq_index2=%d,m_temp_data[freq_index1].freq=%d,m_temp_data[freq_index2].freq=%d\n",freq_index1,freq_index2,m_temp_data[freq_index1].freq,m_temp_data[freq_index2].freq);
//		}
		for(j = min_index; j < max_index; j++)
		{
			/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
			m_temp_data[j].sw_tx_flag[temp_band][port] = TRUE;
			m_temp_data[j].sw_temp_comp[temp_band][port][0] =
					(double)(m_temp_data[freq_index1].sw_temp_comp[temp_band][port][0]) +
					(double)(m_temp_data[freq_index2].sw_temp_comp[temp_band][port][0] - m_temp_data[freq_index1].sw_temp_comp[temp_band][port][0]) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
		}
	}
	return 0;
}

int Temp_config::fill_rx_sw_cal_data(int temp_band, int port)
{
	int i = 0;
	int j = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_temp_data[i].sw_rx_flag[temp_band][port])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].sw_rx_flag[temp_band][port])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].sw_rx_flag[temp_band][port] && j != freq_index1)
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
//		if(temp_band == 0)
//		{
//			printf("rx_swfreq_index1=%d,freq_index2=%d,m_temp_data[freq_index1].freq=%d,m_temp_data[freq_index2].freq=%d\n",freq_index1,freq_index2,m_temp_data[freq_index1].freq,m_temp_data[freq_index2].freq);
//		}
		for(j = min_index; j < max_index; j++)
		{
			/* 公式: y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
			m_temp_data[j].sw_rx_flag[temp_band][port] = TRUE;
			m_temp_data[j].sw_temp_comp[temp_band][port][1] =
					(double)(m_temp_data[freq_index1].sw_temp_comp[temp_band][port][1]) +
					(double)(m_temp_data[freq_index2].sw_temp_comp[temp_band][port][1] - m_temp_data[freq_index1].sw_temp_comp[temp_band][port][1]) /
					(double)(freq_index2 - freq_index1) *
					(double)(j - freq_index1);
		}
	}
	return 0;
}

int Temp_config::fill_cal_data(int temp_band, int port)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_temp_data[i].tx_flag[temp_band][port])
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].tx_flag[temp_band][port])
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(m_temp_data[j].tx_flag[temp_band][port] && j != freq_index1)
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
			/* y = y1 + (y2 - y1) / (x2 - x1) * (x - x1) */
			for(k = 0;  k < TEMP_COLOUM; k++)
			{
				m_temp_data[j].tx_flag[temp_band][port] = TRUE;
				m_temp_data[j].temp_comp[temp_band][port][k] =
						(double)(m_temp_data[freq_index1].temp_comp[temp_band][port][k]) +
						(double)(m_temp_data[freq_index2].temp_comp[temp_band][port][k] - m_temp_data[freq_index1].temp_comp[temp_band][port][k]) /
						(double)(freq_index2 - freq_index1) *
						(double)(j - freq_index1);
			}
		}
	}
	return 0;
}

int Temp_config::show_sw_cal_data(int temp_band, int port)
{
	int i;

	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if((i+FREQ_BASE) > 5800)
			printf("freq=%d,temp_slop tx_slope=%lf,rx_slope=%lf\n",m_temp_data[i].freq,m_temp_data[i].sw_temp_comp[temp_band][port][0],m_temp_data[i].sw_temp_comp[temp_band][port][1]);
	}
	printf("temp_band=%d, port=%d\n",temp_band, port);
	return 0;
}

int Temp_config::show_cal_data(int temp_band, int port)
{
	int i;

	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if((i+FREQ_BASE) <2421 && (i+FREQ_BASE) > 2409)
			printf("freq=%d,0=%lf,1=%lf,2=%lf,3=%lf,4=%lf,5=%lf\n",m_temp_data[i].freq,m_temp_data[i].temp_comp[temp_band][port][0],m_temp_data[i].temp_comp[temp_band][port][1],m_temp_data[i].temp_comp[temp_band][port][2],m_temp_data[i].temp_comp[temp_band][port][3],m_temp_data[i].temp_comp[temp_band][port][4],m_temp_data[i].temp_comp[temp_band][port][5]);
	}
	printf("rf temp_band=%d, port=%d\n",temp_band, port);
	return 0;
}

int Temp_config::show_sw_cal_flag(int temp_band, int port)
{
	int i;

	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(m_temp_data[i].sw_tx_flag[temp_band][port])
		{
			printf("show_sw_cal_flag freq=%d,temp_slop tx_slope=%lf,rx_slope=%lf\n",m_temp_data[i].freq,m_temp_data[i].sw_temp_comp[temp_band][port][0],m_temp_data[i].sw_temp_comp[temp_band][port][1]);
		}
	}
	printf("temp_band=%d, port=%d\n",temp_band, port);
	return 0;
}

int Temp_config::get_csv_line_value(const char *keyword, char *buf, double *value)
{
	char linebuff[KB] = {0};
	char *pstr = buf;

	while(gets_buff_line(linebuff, KB-1, (char**)&pstr) != NULL)
	{
		if (strncmp(linebuff, keyword, strlen(keyword)) == 0)
		{
			int i;
			int len = strlen(linebuff);
			for (i=0; i<len; i++)
			{
				if (linebuff[i] == ',')
				{
					break;
				}
			}
			if ((i == len) || (i == len - 1))
			{
				continue;
			}
			sscanf(&linebuff[i + 1], "%lf", value);
			memset(linebuff, 0, KB);
			break;
		}
		memset(linebuff, 0, KB);
	}
	return 0;
}

/*----------------------------------------------------------------
Name: gets_buff_line
Func: 从ppBuff所指向的buffer中读取一行，以'\n'(0x0a)为分隔符，放在buff中
Para:
InOut:
	buff
	ppBuff:
In
	buff_len
Ret:
	生成的新数据
----------------------------------------------------------------*/
char* Temp_config::gets_buff_line(char* buff, u32 buff_len, char** ppBuff)
{
	char* pTemp;

	pTemp = strchr(*ppBuff, 0xa);
	if (NULL == pTemp)
	{
		return NULL;
	}
	if (buff_len < pTemp - (*ppBuff) + 1)
	{
		memcpy(buff, *ppBuff, buff_len);
	}
	else
	{
		memcpy(buff, *ppBuff, pTemp - (*ppBuff) + 1);
	}
	*ppBuff = pTemp + 1;
	return *ppBuff;
}
