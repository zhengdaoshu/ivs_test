/*
 * att.h
 *
 *  Created on: 2016年11月28日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_ATT_VSG_H_
#define INCLUDE_CAL_MODEL_ATT_VSG_H_
#include "cal_model.h"
#include "../wt-calibration.h"
typedef struct _tx_att_code_value
{
	double att_value[TX_ATT_COUNT][ATT_CODE_MAX];
} tx_att_code_value;

typedef struct _tx_att_data
{
	int freq;	    /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	tx_att_code_value att[UNIT_NUM];
	tx_att_code_value att_pa_off[UNIT_NUM];
} T_tx_att;

class Att_vsg :public Cal_model
{
private:
	T_tx_att * m_cal_data;
	double m_const_gain;
	int share_att4_code;
public:
	Att_vsg(int csv_type);
	~Att_vsg();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

public:
	void set_tx_const_gain(double const_gain);

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_att_value_from_buf(char *file_buf, int len, int device, int unit);
	int get_att_pa_on_calibration_data(char *file_buf);
	int get_att_off_calibration_data();

	int get_att_code(int unit, int device, int freq, double *gain, int is_pa_on);
	int get_new_tune_gain(int unit, int device, int freq, double *gain, int is_pa_on, int att_code);
};

#endif /* INCLUDE_CAL_MODEL_ATT_VSG_H_ */
