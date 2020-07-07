/*
 * tx_iq_imb.h
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_TX_IQ_IMB_H_
#define INCLUDE_CAL_MODEL_TX_IQ_IMB_H_
#include "cal_model.h"
#include "../wt-calibration.h"
#include <map>
using namespace std;

typedef struct _tx_iq_imb_data
{
	int freq;	    /* 频率 索引 */
	BOOL flag[UNIT_NUM];   /* 校准数据填充标记 */
	Tx_Iq_Imb_Parm tx_iq_imb[UNIT_NUM];
} T_tx_iq_imb;

class Tx_iq_imb : public Cal_model
{
private:
	T_tx_iq_imb * m_cal_data;
	map<int, int> m_cal_map;  //5G 频段有多个中频， 为方便插值，引入stl map结构体保存校准索引数据, <freq, int>
public:
	Tx_iq_imb(int csv_type);
	virtual ~Tx_iq_imb();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();


private:
	int init_freq();
	int fill_cal_data(int unit);
	int fill_5g_cal_data(int unit, int freq_index, int CF);

    //*****************************************************************************
    // 函数: get_cal_data_by_freq()
    // 功能：获取最近两个已校准频点，用于进行插值计算， (且需考虑中频因素，当处于中频变化的临界区时，获取同一中频的两个最近频点)
    // 参数[IN]：  freq ，  需插值的频点
    // 返回值：      index，  最近的频点
	//            ch1  ，   最近的频点1
	//            ch2  ，   最近的频点2
    //*****************************************************************************
	int get_cal_data_by_freq(int freq, int *ch1, int *ch2);

	int get_iq_imb_from_file(char *file_buf, int len, int unit);

};

#endif /* INCLUDE_CAL_MODEL_TX_IQ_IMB_H_ */
