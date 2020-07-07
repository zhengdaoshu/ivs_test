/*
 * csv_disk.cpp
 *
 *  Created on: 2016年12月2日
 *      Author: J00054
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>			//标准函数库定义
#include "../../include/csv_file_manager/csv_disk.h"
#include "../../include/base/app_lib.h"

Csv_disk* Csv_disk::getInstance()
{
	static Csv_disk instance;
	return &instance;
}

Csv_disk::Csv_disk() :Csv_file_manager()
{
	m_src_type = src_disk;
}

Csv_disk::~Csv_disk()
{
	;
}

string Csv_disk::GetDir()
{
    char Buf[1024] = {0};
    int Cnt = readlink("/proc/self/exe", Buf, 1024);
    while (Cnt--)
    {
        if (Buf[Cnt] == '/')
        {
            Buf[Cnt] = '\0';
            break;
        }
    }
    return Buf;
}

void Csv_disk::check_dir_status(char* file_path)
{
	int i=0;
	int len = 0;
	char folder_name[32] = {0};
	for(i=0;i<strlen(file_path);i++)
	{
		if(file_path[i] != 47)
		{
			len++;
		}
		else
		{
			break;
		}
	}
	memcpy(folder_name, file_path, len);
	printf("folder_name=%s\n",folder_name);
	if(App_lib::IsFolderExist(folder_name))
	{
		printf("exits %s\n",folder_name);
	}
	else
	{
		char str[256] = {0};
		sprintf(str, "mkdir %s", folder_name);
		system(str);
		printf("not exisit[%s,%d]\n",__FILE__,__LINE__);
	}
}

int Csv_disk::csv_file_get(const char *file_name, char *file_buf)
{
	int len;
	string path = GetDir() + CALIBRATION_FOLDER + file_name;
	len = App_lib::read_file(path.c_str(), file_buf, CAL_FILE_SIZE);
	return len;
}

int Csv_disk::csv_file_set(char *file_name, char *buf, int len)
{
	string path = GetDir() + CALIBRATION_FOLDER + file_name;
	App_lib::write_file(path.c_str(), buf, len);
	return 0;
}

int Csv_disk::set_calibration_data(char *file_name, char *buf, int len)
{
	string path = GetDir() +  "/" + file_name;
	check_dir_status(file_name);
	printf("set_calibration_data len=%d,file_name=%s,path=%s\n",len,file_name,path.c_str());
	App_lib::write_file(path.c_str(), buf, len);
	return 0;
}

int Csv_disk::get_calibration_data(char *file_name, char *buf, int bufsize)
{
	int len = 0;
	string path = GetDir() +  "/" + file_name;
	len = App_lib::read_file(path.c_str(), buf, bufsize);
	return len;
}

int Csv_disk::set_configuration_data(char *file_name, char *buf, int len)
{
	string path = GetDir() + CONFIGURATION_FOLDER + file_name;
	App_lib::write_file(path.c_str(), buf, len);
	return 0;
}

int Csv_disk::get_configuration_data(const char *file_name, char *buf, int size)
{
	int len;
	string path = GetDir() + CONFIGURATION_FOLDER + file_name;
	len = App_lib::read_file(path.c_str(), buf, size);
	return len;
}



