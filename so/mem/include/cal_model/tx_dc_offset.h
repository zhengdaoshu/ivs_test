/*
 * tx_dc_offset.h
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_TX_DC_OFFSET_H_
#define INCLUDE_CAL_MODEL_TX_DC_OFFSET_H_
#include "cal_model.h"
#include "../wt-calibration.h"

typedef struct _tx_dc_offset
{
	Tx_DC_Offset_Parm dc_offset[TX_DC_OFFSET_MAX_COUNT];
} tx_dc_offset_param;

typedef struct _tx_dc_offset_data
{
	int freq;	           /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	tx_dc_offset_param tx_dc_offset[UNIT_NUM];
} T_tx_dc_offset;

class Tx_dc_offset : public Cal_model
{
private:
	T_tx_dc_offset * m_cal_data;
public:
	Tx_dc_offset(int csv_type);
	virtual ~Tx_dc_offset();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_tx_dc_offset_from_file(char *file_buf, int len, int unit);
	int get_tx_dc_offset(int unit, int freq, double dac_gain, Tx_DC_Offset_Parm* ref_tx_dc_offset);
};

#endif /* INCLUDE_CAL_MODEL_TX_DC_OFFSET_H_ */
