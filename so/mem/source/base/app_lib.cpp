/*
 * app_lib.cpp
 *
 *  Created on: 2016��11��24��
 *      Author: J00054
 */
#include <string.h>			//��׼�����������
#include <stdio.h>			//��׼�����������
#include <stdlib.h>			//��׼�����ⶨ��
#include <math.h>			//��׼�����ⶨ��
#include <unistd.h>			//Unix��׼��������
#include<dirent.h>
#include "../../include/base/app_lib.h"
#include "../../include/base/typedef.h"
/*******************************************************************************
����: app_malloc()
����: ����һ��ָ����С���ڴ棬����¼����Ĵ�С
����:
None
����:
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
����: app_free()
����: �ͷ�ָ�����ڴ棬���ҵ���Ӧ���ڴ��ڵ㣬��սڵ���Ϣ�������ͷŽڵ㱾��ռ�õ��ڴ�
����:
None
����:
None
*******************************************************************************/
void App_lib::app_free(char *addr)
{
	free(addr);
}

/*******************************************************************************
����: get_csv_string_item()
����: ��һ���ַ������ҳ�ָ��λ���ַ������ַ����Զ���Ϊ�����index��0��ʼ
����:
����: 0��ʾ��ȡ�ɹ���������ʾ��ȡʧ��
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
		/* ȥ���ո���Ʊ�� */
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
����: get_csv_hex_item()
����: ��һ���ַ������ҳ�ָ��λ�õ�ʮ��������ֵ���ַ����Զ���Ϊ�����index��0��ʼ
����:
����: 0��ʾ��ȡ�ɹ���������ʾ��ȡʧ��
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
		/* ȥ���ո���Ʊ�� */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		sscanf(&buf[i], "%x", value);
	}
	return ret;
}

/*******************************************************************************
����: get_csv_decimal_item()
����: ��һ���ַ������ҳ�ָ��λ�õ�ʮ������ֵ���ַ����Զ���Ϊ�����index��0��ʼ
����:
����: 0��ʾ��ȡ�ɹ���������ʾ��ȡʧ��
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
		/* ȥ���ո���Ʊ�� */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		sscanf(&buf[i], "%d", value);
	}
	return ret;
}

/*******************************************************************************
����: get_csv_float_item()
����: ��һ���ַ������ҳ�ָ��λ�õĸ�����ֵ���ַ����Զ���Ϊ�����index��0��ʼ
����:
����: 0��ʾ��ȡ�ɹ���������ʾ��ȡʧ��
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
		/* ȥ���ո���Ʊ�� */
		while ((buf[i] == ' ') || (buf[i] == '\t'))
		{
			i++;
		}
		sscanf(&buf[i], "%lf", value);
	}
	return ret;
}

/*******************************************************************************
����: get_line()
����: �������������е�ǰ���ǣ�pbuf�е�������'\0'��β
��ppbuf��ָ���buffer�ж�ȡһ�У���'\n'��'\r'Ϊ�ָ���������line��
����:
line: �л�����
len:line�������ɵ�����ֽ���
ppbuf: �ַ���������
����: �����з���0�����򷵻�-1
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
	/* ȥ�����кͻس� */
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
		if (i >= buf_size)	/* �Ѿ��������һ�� */
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
* ������: read_file()
* ����: ��ȡ�ļ�����ָ�����ļ����ݶ���buf�У���ȡ�Ĵ�С��sizeָ��
* ����:
*		path_name: �ļ�·��
*		buf: (���)���ݻ�����
*		size: ��ȡ���ݵĳ���
* ����ֵ: ʵ�ʶ�ȡ�������ݳ���
*******************************************************************************/
int App_lib::read_file(const char *path_name, char *buf, int size)
{
	FILE *fp = NULL;	/* ���ڴ�ָ�����ļ� */
	int ret = 0;

	fp = fopen(path_name, "rb");
	if (fp != NULL)
	{
		memset(buf, 0, size);
		ret = fread(buf, sizeof(char), size, fp); /* ����ֵ�Ƕ�ȡ���������� */
		fclose(fp);
	}
	if (ret <= 0)
	{
	}
	return ret;
}

/* �����ļ���ֱ�Ӹ��ǻ򴴽� */
int App_lib::write_file(const char *path_name, char *buf, int size)
{
	FILE *fp = NULL;	/* ���ڴ�ָ�����ļ� */
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
