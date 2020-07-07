/*
 * app_lib.h
 *
 *  Created on: 2016年11月24日
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
    //给定一个整数A，计算一个整数最接近整数B的倍数的整数
    //参数 [IN]：dA: 数据A  dB: 数据B
    //返回值：最接近整数B的倍数的整数
    //*****************************************************************************
    static int near_num(int A, int B);

    /*----------------------------------------------------------------
    Name: CompareDouble
    Func: 比较两个 double型数据的大小,精度为：0.0001
    Para:
    In
    dA: 数据A,double型
    dB: 数据B，double型
    Ret:
    0: 相等
    1: 不相等
    ----------------------------------------------------------------*/
    static int double_compare(double dA, double dB);

    static void versionStrToArray(char *version, char versionArray[][6]);
    static int fwVersionCmp(char *version1, char *version2);
};

#endif /* INCLUDE_BASE_APP_LIB_H_ */
