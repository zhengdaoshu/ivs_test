/*
 * app_lib.cpp
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */
#include <string.h>			//标准输入输出定义
#include <stdio.h>			//标准输入输出定义
#include <stdlib.h>			//标准函数库定义
#include <math.h>			//标准函数库定义
#include <unistd.h>			//Unix标准函数定义
#include<dirent.h>
#include "../../include/base/app_lib.h"
#include "../../include/base/typedef.h"
/*******************************************************************************
函数: app_malloc()
功能: 申请一块指定大小的内存，并记录申请的大小
参数:
None
返回:
None
*******************************************************************************/
void* App_lib::app_malloc(u32 size)
{
	u8 *mem = NULL;
	mem = (u8*)malloc(size);
	if (mem == NULL)
	{
		exit(0);
	}
	memset(mem, 0, size);
	return (void *)mem;
}

/*******************************************************************************
函数: app_free()
功能: 释放指定的内存，并找到相应的内存块节点，清空节点信息，但不释放节点本身占用的内存
参数:
None
返回:
None
*******************************************************************************/
void App_lib::app_free(char *addr)
{
	free(addr);
}

/*******************************************************************************
函数: get_csv_string_item()
功能: 从一行字符串中找出指定位置字符串，字符串以逗号为间隔，index从0开始
参数:
返回: 0表示获取成功，其他表示获取失败
*******************************************************************************/
int App_lib::get_csv_string_item(char *buf, int index, char *value)
{
	int len = strlen(buf);
	int comma_count = 0;
	int i;
	int ret = 0;

	for (i=0; i<len; i++)
	{
		if (comma_count == index)
		{
			break;
		}
		if (buf[i] == ',')
		{
			comma_count++;
		}
	}
	if (comma_count != index)
	{
		ret = -1;
	}
	else
	{
		char *forward_comma = NULL;
		/* 去掉空格和制表符 */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		forward_comma = strchr(&buf[i], ',');
		if (forward_comma == NULL)
		{
			memcpy(value, &buf[i], len - i);
		}
		else
		{
			memcpy(value, &buf[i], forward_comma - &buf[i]);
		}
	}
	return ret;
}

/*******************************************************************************
函数: get_csv_hex_item()
功能: 从一行字符串中找出指定位置的十六进制数值，字符串以逗号为间隔，index从0开始
参数:
返回: 0表示获取成功，其他表示获取失败
*******************************************************************************/
int App_lib::get_csv_hex_item(char *buf, int index, int *value)
{
	int len = strlen(buf);
	int comma_count = 0;
	int i;
	int ret = 0;

	for (i=0; i<len; i++)
	{
		if (comma_count == index)
		{
			break;
		}
		if (buf[i] == ',')
		{
			comma_count++;
		}
	}
	if (comma_count != index)
	{
		ret = -1;
	}
	else
	{
		/* 去掉空格和制表符 */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		sscanf(&buf[i], "%x", value);
	}
	return ret;
}

/*******************************************************************************
函数: get_csv_decimal_item()
功能: 从一行字符串中找出指定位置的十进制数值，字符串以逗号为间隔，index从0开始
参数:
返回: 0表示获取成功，其他表示获取失败
*******************************************************************************/
int App_lib::get_csv_decimal_item(char *buf, int index, int *value)
{
	int len = strlen(buf);
	int comma_count = 0;
	int i;
	int ret = 0;

	for (i=0; i<len; i++)
	{
		if (comma_count == index)
		{
			break;
		}
		if (buf[i] == ',')
		{
			comma_count++;
		}
	}
	if (comma_count != index)
	{
		ret = -1;
	}
	else
	{
		/* 去掉空格和制表符 */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		sscanf(&buf[i], "%d", value);
	}
	return ret;
}

/*******************************************************************************
函数: get_csv_float_item()
功能: 从一行字符串中找出指定位置的浮点数值，字符串以逗号为间隔，index从0开始
参数:
返回: 0表示获取成功，其他表示获取失败
*******************************************************************************/
int App_lib::get_csv_float_item(char *buf, int index, double *value)
{
	int len = strlen(buf);
	int comma_count = 0;
	int i;
	int ret = 0;

	for (i=0; i<len; i++)
	{
		if (comma_count == index)
		{
			break;
		}
		if (buf[i] == ',')
		{
			comma_count++;
		}
	}
	if (comma_count != index)
	{
		ret = -1;
	}
	else
	{
		/* 去掉空格和制表符 */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		sscanf(&buf[i], "%lf", value);
	}
	return ret;
}

/*******************************************************************************
函数: get_line()
功能: 本函数正常运行的前提是，pbuf中的内容以'\0'结尾
从ppbuf所指向的buffer中读取一行，以'\n'或'\r'为分隔符，放在line中
参数:
line: 行缓冲区
len:line所能容纳的最大字节数
ppbuf: 字符串缓冲区
返回: 存在行返回0，否则返回-1
*******************************************************************************/
int App_lib::get_line(char* line, u32 len, char** ppbuf)
{
	int buf_size = strlen(*ppbuf);
	int ret = 0;
	int i;

	if (buf_size == 0)
	{
		return -1;
	}
	memset(line, 0, len);

	for (i=0; i<buf_size; i++)
	{
		if (((*ppbuf)[i] == '\n') || ((*ppbuf)[i] == '\r'))
		{
			break;
		}
	}
	/* 去掉换行和回车 */
	while (((*ppbuf)[i] == '\n') || ((*ppbuf)[i] == '\r'))
	{
		i++;
	}

	if (i >= len)
	{
		memcpy(line, *ppbuf, len);
		*ppbuf += len;
	}
	else
	{
		if (i >= buf_size)	/* 已经到达最后一行 */
		{
			memcpy(line, *ppbuf, buf_size);
			*ppbuf += buf_size;
		}
		else
		{
			memcpy(line, *ppbuf, i);
			*ppbuf += i;
		}
	}

	return ret;
}

/*******************************************************************************
* 函数名: read_file()
* 功能: 读取文件，将指定的文件内容读到buf中，读取的大小由size指定
* 参数:
*		path_name: 文件路径
*		buf: (输出)内容缓冲区
*		size: 读取数据的长度
* 返回值: 实际读取到的数据长度
*******************************************************************************/
int App_lib::read_file(const char *path_name, char *buf, int size)
{
	FILE *fp = NULL;	/* 用于打开指定的文件 */
	int ret = 0;

	fp = fopen(path_name, "rb");
	if (fp != NULL)
	{
		memset(buf, 0, size);
		ret = fread(buf, sizeof(char), size, fp); /* 返回值是读取的内容数量 */
		fclose(fp);
	}
	if (ret <= 0)
	{
	}
	return ret;
}

/* 保存文件，直接覆盖或创建 */
int App_lib::write_file(const char *path_name, char *buf, int size)
{
	FILE *fp = NULL;	/* 用于打开指定的文件 */
	fp = fopen(path_name, "w+");
	if (fp != NULL)
	{
		fwrite(buf, size, 1, fp);
		fclose(fp);
	}
	return 0;
}

int App_lib::IsFolderExist(const char* path)
{
    DIR *dp;
    if ((dp = opendir(path)) == NULL)
    {
        return 0;
    }

    closedir(dp);
    return 1;
}
int App_lib::near_num(int A, int B)
{
    int left, right, num;

    left = A % B;
    right = B - left;

    num = left > right ? (A + right) : (A - left);

    return num;
}

int App_lib::double_compare(double dA, double dB)
{
	const   double   dEpsilon   =   0.0001;
	if (fabs(dA - dB) < dEpsilon)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

void App_lib::versionStrToArray(char *version, char versionArray[][6])
{
	char *dotAddr = NULL;
	int dotCnt = 0;
	while (1)
	{
		dotAddr = strchr(version, '.');
		if (NULL != dotAddr)
		{
			strncpy(versionArray[dotCnt], version, dotAddr - version);
			dotAddr++;
			version = dotAddr;
			dotCnt++;
		}
		else
		{
			if (version[0] != 0)
			{
				strcpy(versionArray[dotCnt], version);
			}
			break;
		}
	}
}

int App_lib::fwVersionCmp(char *version1, char *version2)
{
	char versionStr1[4][6] = {0};
	char versionStr2[4][6] = {0};
	int i = 0;
	int verTmp1 = 0;
	int verTmp2 = 0;

	printf("version1=%s,version2=%s\n",version1,version2);
	versionStrToArray(version1, versionStr1);
	versionStrToArray(version2, versionStr2);

	for (i = 0; i < 4; i++)
	{
		verTmp1 = atoi(versionStr1[i]);
		verTmp2 = atoi(versionStr2[i]);
		if (verTmp1 > verTmp2)
		{
			return 1;
		}
		else if (verTmp1 < verTmp2)
		{
			return -1;
		}
	}
	if (i == 4)
	{
		return 0;
	}
	return 0;
}
