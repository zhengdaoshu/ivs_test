/*
 * rx_gain_compensation_port.h
 *  �źŴ�ÿһ����Ƶport�˿�����,��Ҫ��������·����
 *  Created on: 2017��1��12��
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_RX_GAIN_COMPENSATION_PORT_H_
#define INCLUDE_CAL_MODEL_RX_GAIN_COMPENSATION_PORT_H_
#include "cal_model.h"

#define RX_COMP_COLOUM (6)
enum RX_PORT_COMPENSATION_TYPE
{
	rx_unit_1_siso = 0,
	rx_unit_1_ac80,          //rx1���ź�ͨ��ac80+80��ʽ����unit 1��·��Ԫ
	rx_unit_1_ac80_link,     //rx1���ź�ͨ��ac80+80��ʽ������һ(unit 2)��·��Ԫ
	rx_unit_2_siso,
	rx_unit_2_ac80,          //rx2���ź�ͨ��ac80+80��ʽ����unit 2��·��Ԫ
	rx_unit_2_ac80_link,     //rx2���ź�ͨ��ac80+80��ʽ������һ(unit 1)��·��Ԫ
};

typedef struct _rx_gain_compensation_parm
{
	double comp_gain[RX_COMP_COLOUM];
} rx_gain_compensation_parm;

typedef struct _rx_gain_compensation_data
{
	int freq;	           /* Ƶ�� ���� */
	BOOL flag[PORT_MAX];   /* У׼��������� */
	rx_gain_compensation_parm rx_gain_compensation[PORT_MAX];
} T_rx_gain_compensation;

class Rx_gain_compensation_port :public Cal_model
{
private:
	T_rx_gain_compensation* m_cal_data;

public:
	Rx_gain_compensation_port(int csv_type);
	~Rx_gain_compensation_port();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int get_port_compensation_value_from_buf(char *file_buf, int len, int port);
	int fill_cal_data(int port);
	int init_freq();
};

#endif /* INCLUDE_CAL_MODEL_RX_GAIN_COMPENSATION_PORT_H_ */
