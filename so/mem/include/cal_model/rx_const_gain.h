/*
 * rx_const_gain.h
 *
 *  Created on: 2017��1��6��
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_RX_CONST_GAIN_H_
#define INCLUDE_CAL_MODEL_RX_CONST_GAIN_H_
#include "cal_model.h"
typedef struct _rx_const_gain_parm
{
	double siso_gain;
	double ac80_gain;          //RF���ź�ͨ��ac80+80��ʽ�������·��Ԫ
	double ac80_link_gain;     //RF���ź�ͨ��ac80+80��ʽ������һ��·��Ԫ
} rx_const_gain_parm;

typedef struct _rx_const_gain_data
{
	int freq;	           /* Ƶ�� ���� */
	BOOL flag[UNIT_NUM];   /* У׼��������� */
	rx_const_gain_parm rx_const_gain[UNIT_NUM];
} T_rx_const_gain;

class Rx_const_gain :public Cal_model
{
private:
	T_rx_const_gain* m_cal_data;
	double m_rx_gain;

public:
	Rx_const_gain(int csv_type);
	~Rx_const_gain();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_rx_gain_from_buf(char *file_buf, int len, int unit);
};



#endif /* INCLUDE_CAL_MODEL_RX_CONST_GAIN_H_ */
