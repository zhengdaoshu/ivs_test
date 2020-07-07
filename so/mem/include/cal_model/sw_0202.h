/*
 * sw_0202.h
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_SW_0202_H_
#define INCLUDE_CAL_MODEL_SW_0202_H_

#include "cal_model.h"
#define SW0202_UMN (4)
#define SW0202_COLUMN (4)
enum sw0202_type
{
	sw2_1_to_1 = 0,
	sw2_1_to_2,
	sw2_2_to_1,
	sw2_2_to_2,
};

typedef struct _sw_0202_value
{
	double loss_value[SW0202_COLUMN];
} sw_0202_value;

typedef struct _sw_0202_data
{
	int freq;	    /* 频率 索引 */
	BOOL flag[SW0202_UMN];   /* 校准数据填充标记 */
	sw_0202_value loss[SW0202_UMN];
} T_sw_0202;

class Sw_0202_loss : public Cal_model
{
private:
	T_sw_0202 * m_cal_data;

public:
	Sw_0202_loss(int csv_type);
	~Sw_0202_loss();
	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int get_sw0202_from_buf(char *file_buf, int len, int unit);
	int get_sw0202_from_device(char *file_buf, int len, int unit);
	int init_freq();
	int fill_cal_data(int unit);

};

#endif /* SOURCE_CAL_MODEL_SW_0202_H_ */
