/*
 * ocxo.cpp
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */
#include<string.h>
#include<stdlib.h>
#include "../../include/cal_model/ocxo.h"
#include "../../include/base/app_lib.h"
#include "../../include/base/basefun.h"
#include "../../include/base/cal_error.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/wt-calibration.h"

Ocxo::Ocxo(int csv_type) :Cal_model(csv_type)
{
	m_ocxo_code = 0;
}
void Ocxo::calibration_data_reset()
{
	m_ocxo_code = 0;
}

int Ocxo::wt_calibration_initial(char *buf)
{
	int len;
	int ret = ERR_RET_OK;
	len = p_csv_manager->csv_file_get("ocxo.csv", buf);
	ret = get_ocxo_code_from_buf(buf, len);
	return ret;
}

double Ocxo::get_calibration_data(char *ref_buf)
{
	Ocxo_Parm *ret = (Ocxo_Parm *)ref_buf;
	ret->ocxo_code1 = m_ocxo_code;
	ret->ocxo_code2 = 16;
	return 0;
}

int Ocxo::get_ocxo_code_from_buf(char *file_buf, int len)
{
#define KEYWORD_SIZE (100)
	char line[LINE_BUFF_LEN] = {0};
	char *pbuf = file_buf;
	char keyword[KEYWORD_SIZE] = {0};
	int ret = 0;

	if (len <= 0)	/* 文件缺失或都打开异常 */
	{
		return ERR_OCXO_CSV_LOST;
	}

	/* 读取配置文件中的值 */
	while (App_lib::get_line(line, LINE_BUFF_LEN, &pbuf) == 0)
	{
		if ((strncmp(line, "##", 2) == 0) || (strncmp(line, "//", 2) == 0))
		{
			continue;
		}
		ret = App_lib::get_csv_string_item(line, 0, keyword);
		err_continue(ret);
		if (!strcmp(keyword, "ocxo"))
		{
			ret = App_lib::get_csv_decimal_item(line, 1, &m_ocxo_code);
		}

		memset(keyword, 0, KEYWORD_SIZE);
	}
	return ret;
}

