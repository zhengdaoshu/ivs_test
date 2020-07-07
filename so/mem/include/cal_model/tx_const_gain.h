/*
 * tx_const_gain.cpp
 *
 *  Created on: 2017年1月6日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_TX_CONST_GAIN_H_
#define INCLUDE_CAL_MODEL_TX_CONST_GAIN_H_
#include "cal_model.h"
typedef struct _tx_const_gain_parm
{
	double siso_pa_gain;
	double siso_gain;
	double ac80_pa_gain;
	double ac80_gain;
	double ac80_link_pa_gain;  //另外一条链路通过ac80+80方式经过该tx端口单元
	double ac80_link_gain;     //放大器关闭
} tx_const_gain_parm;

typedef struct _tx_const_gain_data
{
	int freq;	           /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	tx_const_gain_parm tx_const_gain[UNIT_NUM];
} T_tx_const_gain;

class Tx_const_gain :public Cal_model
{
private:
	T_tx_const_gain* m_cal_data;
	double m_tx_gain;

public:
	Tx_const_gain(int csv_type);
	~Tx_const_gain();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_tx_gain_from_buf(char *file_buf, int len, int unit);
};

#endif /* INCLUDE_CAL_MODEL_TX_CONST_GAIN_CPP_ */
