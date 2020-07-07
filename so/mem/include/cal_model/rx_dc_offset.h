/*
 * rx_dc_offset.h
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_RX_DC_OFFSET_H_
#define INCLUDE_CAL_MODEL_RX_DC_OFFSET_H_
#include "cal_model.h"
#include "../wt-calibration.h"

typedef struct _rx_dc_offset_data
{
	int freq;	           /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	Rx_DC_Offset_Parm rx_dc_offset[UNIT_NUM];
} T_rx_dc_offset;

class Rx_dc_offset : public Cal_model
{
private:
	T_rx_dc_offset* m_cal_data;

public:
	Rx_dc_offset(int csv_type);
	virtual ~Rx_dc_offset();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_rx_dc_offset_from_buf(char *file_buf, int len, int unit);
};




#endif /* INCLUDE_CAL_MODEL_TX_RX_DC_OFFSET_H_ */
