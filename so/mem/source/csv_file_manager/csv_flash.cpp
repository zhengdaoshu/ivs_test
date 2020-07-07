/*
 * csv_flash.cpp
 *
 *  Created on: 2016Äê12ÔÂ2ÈÕ
 *      Author: J00054
 */
#include "../../include/csv_file_manager/csv_flash.h"
#include "../../include/base/app_lib.h"
Csv_flash* Csv_flash::getInstance()
{
	static Csv_flash instance;
	return &instance;
}

Csv_flash::Csv_flash() :Csv_file_manager()
{
	m_src_type = src_flash;
}

Csv_flash::~Csv_flash()
{
	;
}

int Csv_flash::csv_file_get(const char *file_name, char *file_buf)
{
	int len = 0;
	return len;
}

int Csv_flash::csv_file_set(char *file_name, char *buf, int len)
{
	return 0;
}

int Csv_flash::set_calibration_data(char *file_name, char *buf, int len)
{
	return 0;
}

int Csv_flash::get_calibration_data(char *file_name, char *buf, int bufsize)
{
	return 0;
}

