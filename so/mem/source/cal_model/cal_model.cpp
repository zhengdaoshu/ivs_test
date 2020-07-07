/*
 * cal_model.cpp
 *
 *  Created on: 2016Äê12ÔÂ12ÈÕ
 *      Author: J00054
 */
#include<stdlib.h>
#include "../../include/cal_model/cal_model.h"
#include "../../include/csv_file_manager/csv_model.h"
#include "../../include/csv_file_manager/csv_factory.h"
Cal_model::Cal_model(int csv_type)
{
	model_type = CalOcxo;
	m_src_type = csv_type;
	p_csv_manager = Csv_factory::get_csv_manager(m_src_type);
	init_unit_map();
}

Cal_model::~Cal_model()
{
	if(p_csv_manager != NULL)
	{
		delete p_csv_manager;
		p_csv_manager = NULL;
	}
}

void Cal_model::init_unit_map()
{
	unit_map[0] = 0;
	unit_map[1] = 1;
	unit_map[2] = 0;
	unit_map[3] = 1;
	unit_map[4] = 0;
	unit_map[5] = 1;
}

int Cal_model::get_unit_no(int unit_value)
{
	if(unit_value > 5 || unit_value < 0)
	{
		return 0;
	}
	return unit_map[unit_value];
}

int Cal_model::set_csv_type(int csv_type)
{
	m_src_type = csv_type;
	p_csv_manager = Csv_factory::get_csv_manager(m_src_type);
	return 0;
}

int Cal_model::set_calibration_data(char *filename, char *buf, int len)
{
	return p_csv_manager->set_calibration_data(filename, buf, len);
}

int Cal_model::get_calibration_data(char *filename, char *buf, int bufsize)
{
	return p_csv_manager->get_calibration_data(filename, buf, bufsize);
}
