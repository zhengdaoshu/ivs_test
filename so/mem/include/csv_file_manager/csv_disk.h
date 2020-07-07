/*
 * csv_disk.h
 *
 *  Created on: 2016年12月2日
 *      Author: J00054
 */

#ifndef INCLUDE_CSV_FILE_MANAGER_CSV_DISK_H_
#define INCLUDE_CSV_FILE_MANAGER_CSV_DISK_H_
#include "csv_model.h"
#include <string>
using namespace std;
class Csv_disk :public Csv_file_manager
{
public:
	static Csv_disk* getInstance();
private:
	Csv_disk();
	virtual ~Csv_disk();

    //*****************************************************************************
    // 函数: GetDir()
    // 功能：获取可执行程序工作目录(可执行程序由shell脚本启动，而shell脚本与可执行程序可能不在同一个目录)
    // 返回值: string 目录字符串
    //*****************************************************************************
	string GetDir();
	void check_dir_status(char* file_path);
public:
	virtual int csv_file_get(const char *file_name, char *file_buf);
	virtual int csv_file_set(char *file_name, char *buf, int len);
	virtual int set_calibration_data(char *file_name, char *buf, int len);
	virtual int get_calibration_data(char *file_name, char *buf, int bufsize);

	int set_configuration_data(char *file_name, char *buf, int len);
	int get_configuration_data(const char *file_name, char *buf, int size);
};


#endif /* INCLUDE_CSV_FILE_MANAGER_CSV_DISK_H_ */
