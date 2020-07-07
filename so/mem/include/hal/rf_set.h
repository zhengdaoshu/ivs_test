/*
 * rf_set.h
 *
 *  Created on: 2016��11��25��
 *      Author: J00054
 */

#ifndef SOURCE_HAL_RF_SET_H_
#define SOURCE_HAL_RF_SET_H_

#include "../../include/wt-calibration.h"
class Rf_set
{
public:

	Rf_set() {};
    ~Rf_set() {};
    static int is_5g(int freq);
    static int is_2g4(int freq);
    static int is_valid_freq(double dFreq);

    static int get_rx_lo_setting(int unit, double dFreq, Freq_Parm *freq_parm);
    static int get_tx_lo_setting(int unit, double dFreq, Freq_Parm *freq_parm);

    static void init_unit_map();
    static int get_unit_no(int unit_value);

    static int get_tx_if_5g(int freq, int *if_5g);
    static int get_rx_if_5g(int freq, int *if_5g);
    static int get_rx_if_5g_by_index(int if_5g_index);
    static int get_tx_if_5g_by_index(int if_5g_index);

    //*****************************************************************************
    // ����: compare_ifg()
    // ���ܣ��Ƚ�����Ƶ���Ƿ�ͬ��һ����Ƶ
    // ����[IN] : freq1
	//            freq2
    // ����ֵ�� 0 ͬ��һ����Ƶ
    //         1 ����ͬһ����Ƶ
    //*****************************************************************************
    static int compare_ifg(int freq1, int freq2, int mode);

};

#endif /* SOURCE_HAL_RF_SET_H_ */
