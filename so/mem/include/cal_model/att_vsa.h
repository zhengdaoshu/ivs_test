/*
 * att_vsa.h
 *
 *  Created on: 2016年11月28日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_ATT_H_
#define INCLUDE_CAL_MODEL_ATT_H_
#include "cal_model.h"
#include "../wt-calibration.h"

typedef struct _rx_att_code_value
{
	double att_value[RX_ATT_COUNT][ATT_CODE_MAX];
} rx_att_value;

typedef struct _rx_att_data
{
	int freq;	    /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	rx_att_value att[UNIT_NUM];
} T_rx_att;

class Att_vsa :public Cal_model
{
private:
	T_rx_att * m_cal_data;
	double m_const_gain;
	int share_att1_code;
	int share_att2_code;

public:
	Att_vsa(int csv_type);
	~Att_vsa();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

public:
	void set_rx_const_gain(double const_gain);
	double get_current_default_gain();

	void get_cal_mode_data(char *ref_buf);

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_att_value_from_buf(char *file_buf, int len, int device, int unit);
	int get_att_pa_on_calibration_data(char *file_buf);

	int get_att_code(int unit, int device, int freq, double *gain);
	int get_new_tune_gain(int unit, int device, int freq, double *gain, int att_code);
};

#endif /* INCLUDE_CAL_MODEL_ATT_H_ */
