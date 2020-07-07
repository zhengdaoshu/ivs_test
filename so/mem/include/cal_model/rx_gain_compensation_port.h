/*
 * rx_gain_compensation_port.h
 *  信号从每一个射频port端口输入,需要补偿的线路增益
 *  Created on: 2017年1月12日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_RX_GAIN_COMPENSATION_PORT_H_
#define INCLUDE_CAL_MODEL_RX_GAIN_COMPENSATION_PORT_H_
#include "cal_model.h"

#define RX_COMP_COLOUM (6)
enum RX_PORT_COMPENSATION_TYPE
{
	rx_unit_1_siso = 0,
	rx_unit_1_ac80,          //rx1口信号通过ac80+80方式进入unit 1链路单元
	rx_unit_1_ac80_link,     //rx1口信号通过ac80+80方式进入另一(unit 2)链路单元
	rx_unit_2_siso,
	rx_unit_2_ac80,          //rx2口信号通过ac80+80方式进入unit 2链路单元
	rx_unit_2_ac80_link,     //rx2口信号通过ac80+80方式进入另一(unit 1)链路单元
};

typedef struct _rx_gain_compensation_parm
{
	double comp_gain[RX_COMP_COLOUM];
} rx_gain_compensation_parm;

typedef struct _rx_gain_compensation_data
{
	int freq;	           /* 频率 索引 */
	BOOL flag[PORT_MAX];   /* 校准数据填充标记 */
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
