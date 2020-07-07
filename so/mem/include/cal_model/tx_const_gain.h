/*
 * tx_const_gain.cpp
 *
 *  Created on: 2017��1��6��
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
	double ac80_link_pa_gain;  //����һ����·ͨ��ac80+80��ʽ������tx�˿ڵ�Ԫ
	double ac80_link_gain;     //�Ŵ����ر�
} tx_const_gain_parm;

typedef struct _tx_const_gain_data
{
	int freq;	           /* Ƶ�� ���� */
	BOOL flag[UNIT_NUM];   /* У׼��������� */
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
