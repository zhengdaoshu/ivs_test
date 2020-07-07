/*
 * app_lib.h
 *
 *  Created on: 2016��11��24��
 *      Author: J00054
 */

#ifndef INCLUDE_BASE_APP_LIB_H_
#define INCLUDE_BASE_APP_LIB_H_

#include "typedef.h"
class App_lib
{
public:
	App_lib() {};
    ~App_lib() {};

    static void *app_malloc(u32 size);

    static void app_free(char *addr);

    static int get_csv_string_item(char *buf, int index, char *value);

    static int get_csv_hex_item(char *buf, int index, int *value);

    static int get_csv_decimal_item(char *buf, int index, int *value);

    static int get_csv_float_item(char *buf, int index, double *value);

    static int get_line(char* line, u32 len, char** ppbuf);

    static int read_file(const char *path_name, char *buf, int size);

    static int write_file(const char *path_name, char *buf, int size);

    static int IsFolderExist(const char* path);

    //*****************************************************************************
    //����һ������A������һ��������ӽ�����B�ı���������
    //���� [IN]��dA: ����A  dB: ����B
    //����ֵ����ӽ�����B�ı���������
    //*****************************************************************************
    static int near_num(int A, int B);

    /*----------------------------------------------------------------
    Name: CompareDouble
    Func: �Ƚ����� double�����ݵĴ�С,����Ϊ��0.0001
    Para:
    In
    dA: ����A,double��
    dB: ����B��double��
    Ret:
    0: ���
    1: �����
    ----------------------------------------------------------------*/
    static int double_compare(double dA, double dB);

    static void versionStrToArray(char *version, char versionArray[][6]);
    static int fwVersionCmp(char *version1, char *version2);
};

#endif /* INCLUDE_BASE_APP_LIB_H_ */
