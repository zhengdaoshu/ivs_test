/*
 * tx_iq_imb.h
 *
 *  Created on: 2016��11��24��
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
	int freq;	    /* Ƶ�� ���� */
	BOOL flag[UNIT_NUM];   /* У׼��������� */
	Tx_Iq_Imb_Parm tx_iq_imb[UNIT_NUM];
} T_tx_iq_imb;

class Tx_iq_imb : public Cal_model
{
private:
	T_tx_iq_imb * m_cal_data;
	map<int, int> m_cal_map;  //5G Ƶ���ж����Ƶ�� Ϊ�����ֵ������stl map�ṹ�屣��У׼��������, <freq, int>
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
    // ����: get_cal_data_by_freq()
    // ���ܣ���ȡ���������У׼Ƶ�㣬���ڽ��в�ֵ���㣬 (���迼����Ƶ���أ���������Ƶ�仯���ٽ���ʱ����ȡͬһ��Ƶ���������Ƶ��)
    // ����[IN]��  freq ��  ���ֵ��Ƶ��
    // ����ֵ��      index��  �����Ƶ��
	//            ch1  ��   �����Ƶ��1
	//            ch2  ��   �����Ƶ��2
    //*****************************************************************************
	int get_cal_data_by_freq(int freq, int *ch1, int *ch2);

	int get_iq_imb_from_file(char *file_buf, int len, int unit);

};

#endif /* INCLUDE_CAL_MODEL_TX_IQ_IMB_H_ */
