/*
 * bbvga.cpp
 *
 *  Created on: 2017年1月5日
 *      Author: J00054
 */

#include<string.h>
#include<stdlib.h>
#include "../../include/cal_model/bbvga.h"
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/csv_file_manager/csv_model.h"

Bbvga::Bbvga(int csv_type) :Cal_model(csv_type)
{
	memset(m_cal_data, 0, sizeof(m_cal_data));
}

void Bbvga::calibration_data_reset()
{
	memset(m_cal_data, 0, sizeof(m_cal_data));
}

int Bbvga::wt_calibration_initial(char *buf)
{
	int len;
	int ret = ERR_RET_OK;
	len = p_csv_manager->csv_file_get("bbvga.csv", buf);
	ret = get_bbvga_from_buf(buf, len);
	return ret;
}

double Bbvga::get_calibration_data(char *ref_buf)
{
	return m_cal_data[0];
}

int Bbvga::get_bbvga_from_buf(char *file_buf, int len)
{
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	int ret = 0;
	int i;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_BBVGA_CSV_LOST;
	}
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}

		for (i=0; i<BBVGA_CODE_MAX; i++)
		{
			ret = App_lib::get_csv_float_item(line, i, &m_cal_data[i]);
			err_break(ret);
		}
	}
	return ret;
}
