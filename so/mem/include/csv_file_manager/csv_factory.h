/*
 * csv_simple_factory.h
 * 创建csv文件管理简单工厂类
 *
 *  Created on: 2016年12月3日
 *      Author: J00054
 */

#ifndef INCLUDE_CSV_FILE_MANAGER_CSV_FACTORY_H_
#define INCLUDE_CSV_FILE_MANAGER_CSV_FACTORY_H_
#include "csv_model.h"
class Csv_factory
{
public:
	static Csv_file_manager* get_csv_manager(int src_type);
};

#endif /* INCLUDE_CSV_FILE_MANAGER_CSV_FACTORY_H_ */
