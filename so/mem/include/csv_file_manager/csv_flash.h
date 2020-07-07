/*
 * csv_flash.h
 *
 *  Created on: 2016Äê12ÔÂ2ÈÕ
 *      Author: J00054
 */

#ifndef INCLUDE_CSV_FILE_MANAGER_CSV_FLASH_H_
#define INCLUDE_CSV_FILE_MANAGER_CSV_FLASH_H_
#include "csv_model.h"
class Csv_flash :public Csv_file_manager
{
public:
	static Csv_flash* getInstance();

private:
	Csv_flash();
	virtual ~Csv_flash();

public:
	virtual int csv_file_get(const char *file_name, char *file_buf);
	virtual int csv_file_set(char *file_name, char *buf, int len);
	virtual int set_calibration_data(char *file_name, char *buf, int len);
	virtual int get_calibration_data(char *file_name, char *buf, int bufsize);
};

#endif /* INCLUDE_CSV_FILE_MANAGER_CSV_FLASH_H_ */
