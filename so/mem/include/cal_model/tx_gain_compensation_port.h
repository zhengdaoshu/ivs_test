/*
 * tx_gain_compensation_port.h
 *  �źŴ�ÿһ����Ƶport�˿����,��Ҫ��������·����
 *  Created on: 2017��1��10��
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_TX_GAIN_COMPENSATION_PORT_H_
#define INCLUDE_CAL_MODEL_TX_GAIN_COMPENSATION_PORT_H_
#include "cal_model.h"

#define TX_COMP_COLOUM (12)
enum TX_PORT_COMPENSATION_TYPE
{
	tx_unit_1_pa_on = 0,
	tx_unit_1_pa_off,
	tx_unit_1_ac80_pa,
	tx_unit_1_ac80,
	tx_unit_1_ac80_link_pa, //����һ����·ͨ��ac80+80���ؾ�����tx�˿ڵ�Ԫ
	tx_unit_1_ac80_link,
	tx_unit_2_pa_on,
	tx_unit_2_pa_off,
	tx_unit_2_ac80_pa,
	tx_unit_2_ac80,
	tx_unit_2_ac80_link_pa, //����һ����·ͨ��ac80+80���ؾ�����tx�˿ڵ�Ԫ
	tx_unit_2_ac80_link,
};

typedef struct _tx_gain_compensation_parm
{
	double comp_gain[TX_COMP_COLOUM];
} tx_gain_compensation_parm;

typedef struct _tx_gain_compensation_data
{
	int freq;	           /* Ƶ�� ���� */
	BOOL flag[PORT_MAX];   /* У׼��������� */
	tx_gain_compensation_parm tx_gain_compensation[PORT_MAX];
} T_tx_gain_compensation;

class Tx_gain_compensation_port :public Cal_model
{
private:
	T_tx_gain_compensation* m_cal_data;

public:
	Tx_gain_compensation_port(int csv_type);
	~Tx_gain_compensation_port();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int get_port_compensation_value_from_buf(char *file_buf, int len, int port);
	int fill_cal_data(int port);
	int init_freq();
};


#endif /* INCLUDE_CAL_MODEL_TX_GAIN_COMPENSATION_PORT_H_ */
