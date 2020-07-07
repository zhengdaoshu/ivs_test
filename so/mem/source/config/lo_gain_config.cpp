/*
 * lo_gain_config.cpp
 *
 *  Created on: 2017年1月21日
 *      Author: J00054
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../include/config/lo_gain_config.h"
#include "../../include/base/app_lib.h"
#include "../../include/cal_model/cal_model.h"
#include "../../include/csv_file_manager/csv_factory.h"
#include "../../include/hal/cal_data.h"
#include "../../include/base/cal_error.h"
#include "../../include/base/basefun.h"

Lo_gain* hmc_lo_gain = NULL;

Lo_gain_config::Lo_gain_config()
{
	hmc_lo_gain = (Lo_gain *)App_lib::app_malloc(MAX_CHANNEL_COUNT * sizeof(Lo_gain));
	p_csv_manager = (Csv_disk*)Csv_factory::get_csv_manager(src_disk);
	init_freq();
}

Lo_gain_config::~Lo_gain_config()
{
	if(hmc_lo_gain != NULL)
	{
	    delete[] hmc_lo_gain;
	    hmc_lo_gain = NULL;
	}
}

int Lo_gain_config::lo_configuration_initial(char *buf)
{
	int len;
	int ret = ERR_RET_OK;
	len = p_csv_manager->get_configuration_data("lo-gain.csv", buf, CAL_FILE_SIZE);
	ret = get_lo_gain_from_file(buf, len);
	fill_cal_data();
	return ret;
}

/*******************************************************************************
函数: get_lo_gain_from_file()
功能: 获取LO-Gain
参数:
返回: 获取成功返回0，错误返回-1
*******************************************************************************/
int Lo_gain_config::get_lo_gain_from_file(char *file_buf, int len)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int freq = 0;
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_LO_GAIN_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_decimal_item(line, 0, &freq);
		err_continue(ret);
		hmc_lo_gain[freq-FREQ_BASE].flag = TRUE;
		App_lib::get_csv_decimal_item(line, 1, &hmc_lo_gain[freq-FREQ_BASE].value[0]);
		App_lib::get_csv_decimal_item(line, 2, &hmc_lo_gain[freq-FREQ_BASE].value[1]);
		App_lib::get_csv_decimal_item(line, 3, &hmc_lo_gain[freq-FREQ_BASE].value[2]);
		App_lib::get_csv_decimal_item(line, 4, &hmc_lo_gain[freq-FREQ_BASE].value[3]);
	}
	return ret;
}

int Lo_gain_config::init_freq()
{
	int i = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		hmc_lo_gain[i].freq = FREQ_BASE + i;
	}
	return 0;
}

int Lo_gain_config::fill_cal_data()
{
	int i = 0;
	int j = 0;
	int freq_index1 = 0;
	int freq_index2 = 0;
	int min_index = 0;
	int max_index = 0;
	for(i = 0; i< MAX_CHANNEL_COUNT; i++)
	{
		if(hmc_lo_gain[i].flag)
		{
			continue;
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(hmc_lo_gain[j].flag)
			{
				freq_index1 = j;
				break;
			}
		}
		for(j = (i >= 1 ? i-1 : i); j < MAX_CHANNEL_COUNT; j++)
		{
			if(hmc_lo_gain[j].flag && j != freq_index1)
			{
				freq_index2 = j;
				break;
			}
		}
		//printf("i=%d,freq_index1=%d,freq_index2=%d\n",i,freq_index1,freq_index2);
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
			hmc_lo_gain[j].flag = TRUE;
			hmc_lo_gain[j].value[0] = hmc_lo_gain[freq_index1].value[0];
			hmc_lo_gain[j].value[1] = hmc_lo_gain[freq_index1].value[1];
			hmc_lo_gain[j].value[2] = hmc_lo_gain[freq_index1].value[2];
			hmc_lo_gain[j].value[3] = hmc_lo_gain[freq_index1].value[3];
		}
	}
	return 0;
}

