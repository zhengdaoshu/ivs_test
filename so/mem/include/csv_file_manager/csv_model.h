/*
 * csv_model.h
 *
 *  Created on: 2016Äê12ÔÂ2ÈÕ
 *      Author: J00054
 */

#ifndef INCLUDE_CSV_FILE_MANAGER_CSV_MODEL_H_
#define INCLUDE_CSV_FILE_MANAGER_CSV_MODEL_H_

#define PATH_SIZE (100)
#define CAL_FILE_SIZE (MB)
#define LINE_BUFF_LEN (10240)
#define CALIBRATION_FOLDER			"/calibration/"
#define CONFIGURATION_FOLDER		"/configuration/"
enum csv_file_src
{
	src_disk  = 0,
	src_flash = 1,
};

class Csv_file_manager
{
protected:
	int m_src_type;
public:
	Csv_file_manager(){m_src_type = src_disk;};
	virtual ~Csv_file_manager(){};
	virtual int csv_file_get(const char *file_name, char *buf) = 0;
	virtual int csv_file_set(char *file_name, char *buf, int len) = 0;

	virtual int set_calibration_data(char *file_name, char *buf, int len) = 0;
	virtual int get_calibration_data(char *file_name, char *buf, int bufsize) = 0;
};

#endif /* INCLUDE_CSV_FILE_MANAGER_CSV_MODEL_H_ */
