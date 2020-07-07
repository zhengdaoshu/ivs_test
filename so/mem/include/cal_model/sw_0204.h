/*
 * sw0204.h
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_SW_0204_H_
#define INCLUDE_CAL_MODEL_SW_0204_H_
#include "cal_model.h"
#define SW0204_COLUMN (8)
enum sw0204_type
{
	sw4_1_to_1 = 0,
	sw4_1_to_2,
	sw4_1_to_3,
	sw4_1_to_4,
	sw4_2_to_1,
	sw4_2_to_2,
	sw4_2_to_3,
	sw4_2_to_4,
};

typedef struct _sw_0204_value{
	double loss_value[SW0204_COLUMN];
} sw_0204_value;

typedef struct _sw_0204_data{
	int freq;	    /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	sw_0204_value loss[UNIT_NUM];
} T_sw_0204;

class Sw_0204_loss : public Cal_model
{
private:
	T_sw_0204 * m_cal_data;
public:
	Sw_0204_loss(int csv_type);
	~Sw_0204_loss();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int get_sw0204_from_buf(char *file_buf, int len, int unit);
	int get_sw0204_from_device(char *file_buf, int len, int unit);
	int init_freq();
	int fill_cal_data(int unit);

};

#endif /* INCLUDE_CAL_MODEL_SW_0204_H_ */
