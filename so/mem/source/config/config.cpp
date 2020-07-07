/*
 * conf.cpp
 *
 *  Created on: 2016年12月13日
 *      Author: J00054
 */
#include <string.h>
#include <stdlib.h>
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/csv_file_manager/csv_factory.h"
#include "../../include/config/config.h"
Rf_Config_Data Rf_Config;
Cal_Opt Cal_config;

WTConfig::WTConfig()
{
	memset(&Rf_Config, 0, sizeof(Rf_Config_Data));
	memset(&Cal_config, 0, sizeof(Cal_Opt));
	p_csv_manager = (Csv_disk*)Csv_factory::get_csv_manager(src_disk);
	lo_gain_config = new Lo_gain_config();
}

WTConfig::~WTConfig()
{
	if(lo_gain_config != NULL)
	{
		delete lo_gain_config;
		lo_gain_config = NULL;
	}
}

int WTConfig::wt_configuration_initial(char *buf)
{
	int len = 0;
	int ret = 0;
	do
	{
		len = p_csv_manager->get_configuration_data("rf_config.csv", buf, CAL_FILE_SIZE);
		ret = get_rf_conig_from_buf(buf, len);
		err_break(ret);

		len = p_csv_manager->get_configuration_data("wt_config.csv", buf, CAL_FILE_SIZE);
		ret = get_wt_conig_from_buf(buf, len);
		err_break(ret);

		ret = lo_gain_config->lo_configuration_initial(buf);
	}while(0);

	printf("ret=%d,dac_margin=%lf,dac_margin_ax=%lf,tx_pa_threshold_2g=%lf,adc_expected_power=%lf,Rf_Config.flatness_vsg_version=%d,temprature=%d\n",ret,Rf_Config.dac_margin,Rf_Config.dac_margin_ax,Rf_Config.tx_pa_threshold_2g,Rf_Config.adc_expected_power,Rf_Config.flatness_vsg_version, Rf_Config.tx_temp_cps);
	printf("temp_subband1=%lf,temp_subband2=%lf,temp_subband3=%lf,temp_subband4=%lf\n",Rf_Config.temp_subband1,Rf_Config.temp_subband2,Rf_Config.temp_subband3,Rf_Config.temp_subband4);
	return ret;
}

int WTConfig::get_rf_configuration(Rf_Config_Parm *rf_config_parm)
{
	rf_config_parm->dac_margin = Rf_Config.dac_margin_ax;  //VB板暂时调试用
	rf_config_parm->cal_port_temp_avg = Rf_Config.cal_port_temp_avg;
	return 0;
}

int WTConfig::set_calibration_option(int buf_size, void *cal_opt)
{
	if(buf_size != sizeof(Cal_Opt))
	{
		printf("ERR_CAL_CONFIG\n");
		return ERR_CAL_CONFIG;
	}
	memcpy(&Cal_config, cal_opt, sizeof(Cal_Opt));

	if(App_lib::double_compare(Cal_config.tx_pa_threshold_2g, 0.0))
	{
		Rf_Config.tx_pa_threshold_2g = Cal_config.tx_pa_threshold_2g;
		Rf_Config.tx_pa_threshold_5g = Cal_config.tx_pa_threshold_5g;
	}

	Rf_Config.tx_temp_cps = Cal_config.tx_temp_comp_enable;
	Rf_Config.rx_temp_cps = Cal_config.rx_temp_comp_enable;
	Rf_Config.calibration_flat = Cal_config.calibration_flat;
	return 0;
}

int WTConfig::get_calibration_option(int len, void *cal_opt)
{
	Cal_Opt *cal_config = (Cal_Opt *)cal_opt;
	cal_config->tx_pa_threshold_2g = Rf_Config.tx_pa_threshold_2g;
	cal_config->tx_pa_threshold_5g = Rf_Config.tx_pa_threshold_5g;
	cal_config->tx_temp_comp_enable = Rf_Config.tx_temp_cps;
	cal_config->rx_temp_comp_enable = Rf_Config.rx_temp_cps;
	cal_config->calibration_flat = Rf_Config.calibration_flat;
	//printf("get_calibration_option unit=%d,unittype=%d,tx_pa_threshold_2g=%lf\n",((Cal_Opt *)cal_opt)->unit,((Cal_Opt *)cal_opt)->unit_type,((Cal_Opt *)cal_opt)->tx_pa_threshold_2g);
	return sizeof(Cal_Opt);
}

int WTConfig::set_configuration_data(char *filename, char *buf, int len)
{
	return p_csv_manager->set_configuration_data(filename, buf, len);
}

int WTConfig::get_configuration_data(char *filename, char *buf, int size)
{
	return p_csv_manager->get_configuration_data(filename, buf, size);
}

int WTConfig::get_rf_conig_from_buf(char *buf, int len)
{
#define KEYWORD_SIZE (100)
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = buf;
	char keyword[KEYWORD_SIZE] = {0};
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_RF_CONFIG_CSV_LOST;
	}

	Rf_Config.rx_subband6 = 0;

	/* 读取配置文件中的值 */
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_string_item(line, 0, keyword);
		err_continue(ret);
		if (!strcmp(keyword, "TX-Subband1"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_subband1);
		else if (!strcmp(keyword, "TX-Subband2"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_subband2);
		else if (!strcmp(keyword, "TX-Subband3"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_subband3);
		else if (!strcmp(keyword, "TX-Subband4"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_subband4);
		else if (!strcmp(keyword, "TX-Subband5"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_subband5);
		else if (!strcmp(keyword, "TX-IF1"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_if_1);
		else if (!strcmp(keyword, "TX-IF2"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_if_2);
		else if (!strcmp(keyword, "TX-IF3"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_if_3);
		else if (!strcmp(keyword, "TX-IF4"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_if_4);
		else if (!strcmp(keyword, "TX-IF5"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_if_5);
		else if (!strcmp(keyword, "RX-Subband1"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_subband1);
		else if (!strcmp(keyword, "RX-Subband2"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_subband2);
		else if (!strcmp(keyword, "RX-Subband3"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_subband3);
		else if (!strcmp(keyword, "RX-Subband4"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_subband4);
		else if (!strcmp(keyword, "RX-Subband5"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_subband5);
		else if (!strcmp(keyword, "RX-Subband6"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_subband6);
		else if (!strcmp(keyword, "RX-IF1"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_if_1);
		else if (!strcmp(keyword, "RX-IF2"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_if_2);
		else if (!strcmp(keyword, "RX-IF3"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_if_3);
		else if (!strcmp(keyword, "RX-IF4"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_if_4);
		else if (!strcmp(keyword, "RX-IF5"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_if_5);
		else if (!strcmp(keyword, "RX-IF6"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_if_6);
		else if (!strcmp(keyword, "RX-Att-1-2g4-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_att_2g4_code[0]);
		else if (!strcmp(keyword, "RX-Att-2-2g4-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_att_2g4_code[1]);
		else if (!strcmp(keyword, "RX-Att-1-5g-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_att_5g_code[0]);
		else if (!strcmp(keyword, "RX-Att-2-5g-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.rx_att_5g_code[1]);
		else if (!strcmp(keyword, "TX-Att-1-2g4-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_on_code[0]);
		else if (!strcmp(keyword, "TX-Att-2-2g4-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_on_code[1]);
		else if (!strcmp(keyword, "TX-Att-3-2g4-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_on_code[2]);
		else if (!strcmp(keyword, "TX-Att-4-2g4-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_on_code[3]);
		else if (!strcmp(keyword, "TX-Att-1-5g-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_on_code[0]);
		else if (!strcmp(keyword, "TX-Att-2-5g-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_on_code[1]);
		else if (!strcmp(keyword, "TX-Att-3-5g-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_on_code[2]);
		else if (!strcmp(keyword, "TX-Att-4-5g-PA"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_on_code[3]);
		else if (!strcmp(keyword, "TX-Att-1-2g4-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_off_code[0]);
		else if (!strcmp(keyword, "TX-Att-2-2g4-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_off_code[1]);
		else if (!strcmp(keyword, "TX-Att-3-2g4-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_off_code[2]);
		else if (!strcmp(keyword, "TX-Att-4-2g4-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_2g4_pa_off_code[3]);
		else if (!strcmp(keyword, "TX-Att-1-5g-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_off_code[0]);
		else if (!strcmp(keyword, "TX-Att-2-5g-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_off_code[1]);
		else if (!strcmp(keyword, "TX-Att-3-5g-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_off_code[2]);
		else if (!strcmp(keyword, "TX-Att-4-5g-PA-OFF"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_att_5g_pa_off_code[3]);
		else if (!strcmp(keyword, "Flatness-vsa-version"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.flatness_vsa_version);
		else if (!strcmp(keyword, "Flatness-vsg-version"))
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.flatness_vsg_version);
		memset(keyword, 0, KEYWORD_SIZE);
	}
	return ret;
}

int WTConfig::get_wt_conig_from_buf(char *buf, int len)
{
#define KEYWORD_SIZE (100)
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = buf;
	char keyword[KEYWORD_SIZE] = {0};
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_WT_CONFIG_CSV_LOST;
	}
	/* 默认值初始化 */
	Rf_Config.tx_pa_threshold_2g = -30.0;
	Rf_Config.tx_pa_threshold_5g = -30.0;
	Rf_Config.dac_margin_ax = 15.0;
	/* 读取配置文件中的值 */
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_string_item(line, 0, keyword);
		err_continue(ret);
		if (!strcmp(keyword, "ADC-expected-input"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.adc_expected_power);
		else if (!strcmp(keyword, "DAC-Margin"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.dac_margin);
		else if (!strcmp(keyword, "DAC-Margin-AX"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.dac_margin_ax);
		else if (!strcmp(keyword, "TX-PA-threshold-2G"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.tx_pa_threshold_2g);
		else if (!strcmp(keyword, "TX-PA-threshold-5G"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.tx_pa_threshold_5g);
		else if (!strcmp(keyword, "Temperature-compensation"))
		{
			ret = App_lib::get_csv_decimal_item(line, 1, &Rf_Config.tx_temp_cps);
			Rf_Config.rx_temp_cps = Rf_Config.tx_temp_cps;
		}
		else if (!strcmp(keyword, "Temp-Subband1"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.temp_subband1);
		else if (!strcmp(keyword, "Temp-Subband2"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.temp_subband2);
		else if (!strcmp(keyword, "Temp-Subband3"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.temp_subband3);
		else if (!strcmp(keyword, "Temp-Subband4"))
			ret = App_lib::get_csv_float_item(line, 1, &Rf_Config.temp_subband4);
		memset(keyword, 0, KEYWORD_SIZE);
	}
	return ret;
}
