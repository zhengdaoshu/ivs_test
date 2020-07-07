/*
 * lo_gain_config.h
 *
 *  Created on: 2017��1��21��
 *      Author: J00054
 */

#ifndef INCLUDE_CONFIG_LO_GAIN_CONFIG_H_
#define INCLUDE_CONFIG_LO_GAIN_CONFIG_H_
#include "../../include/base/typedef.h"
#include "../csv_file_manager/csv_disk.h"

#define HMC833_COUNT 4
typedef struct _lo_gain
{
	int freq;
	BOOL flag;
	int value[HMC833_COUNT];	/* ����Ĺ��ʣ���Ϊ4������3��ʾMax gain��2��ʾMax gain - 3dB��... 0��ʾMax gain - 9dB */
} Lo_gain;

class Lo_gain_config
{
private:
	Csv_disk *p_csv_manager;

public:
	Lo_gain_config();
	virtual ~Lo_gain_config();

	int lo_configuration_initial(char *buf);


private:
	int init_freq();
	int fill_cal_data();
	int get_lo_gain_from_file(char *file_buf, int len);
};

#endif /* INCLUDE_CONFIG_LO_GAIN_CONFIG_H_ */
