/*
 * csv_simple_factory.h
 * ����csv�ļ�����򵥹�����
 *
 *  Created on: 2016��12��3��
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
