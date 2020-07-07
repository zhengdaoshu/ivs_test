/*
 * rx_response.h
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_RX_RESPONSE_H_
#define INCLUDE_CAL_MODEL_RX_RESPONSE_H_
#include "cal_model.h"
#include "../wt-calibration.h"

#define RX_SPAN (769)
#define RX_SPAN_OLD (241)
//中频个数
#define RX_IF_COUNT (6)
typedef struct _rx_rf_gain
{
	double rf_gain[PORT_MAX][RX_IF_COUNT];  /* 射频功率，8个端口， 5个中频 */
	double ac80_host[PORT_MAX][RX_IF_COUNT]; /* ac80_80主机 */
	double ac80_slave[PORT_MAX][RX_IF_COUNT];/* ac80_80从机 */
} T_rx_rf_gain;

typedef struct _rx_bb_gain
{
	double bb_gain[UNIT_NUM][RX_IF_COUNT];  /* 基带功率，2个单元， 5个中频 */
} T_rx_bb_gain;

typedef struct _rx_const_gain_response_data
{
	int freq;	           /* 频率 索引 */
	T_rx_bb_gain rx_bb_gain;
	T_rx_rf_gain rx_rf_gain;
} T_rx_response_data;


class Rx_response :public Cal_model
{
private:
	T_rx_response_data* m_cal_data;
	double m_sample_freq; //采样率
	int data_flag;   //支持ac_80_80平坦度校准数据

public:
	Rx_response(int csv_type);
	~Rx_response();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

public:
	void set_rx_sample_freq(double sample_freq);

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_rx_rf_response_data(char *file_buf);
	int get_rx_bb_response_data(char *file_buf);
	int get_rx_rf_response_from_buf(char *file_buf, int len, int unit);
	int get_rx_bb_response_from_buf(char *file_buf, int len, int unit);
    //*****************************************************************************
    // 函数: rx_spectral_flatness_compensate()
    // 功能：获取VSA平坦度补偿数据
    // 参数[IN] : unit : 单元编号
	//            double sample_freq 采样率
	//            CF  中心频率 channel freq
	//            port 端口号
	// 参数[OUT] : rx_flat_comp_parm : 插值计算后的平坦度补偿数据
    // 返回值: 0
    //*****************************************************************************
	int rx_spectral_flatness_compensate(int unit, double sample_freq, int CF, int port, Rx_Spec_Flat_Comp_Parm *rx_flat_comp_parm);
};


struct rx_response_item
{
	double gain[RX_SPAN];
	double host_gain[RX_SPAN];
	double slave_gain[RX_SPAN];
};

class Rx_response_v2 : public Cal_model
{
private:
	rx_response_item *m_cal_data[PORT_MAX];
	double m_sample_freq; //采样率
	int data_flag;        //支持ac_80_80平坦度校准数据

public:
	Rx_response_v2(int csv_type);
	virtual ~Rx_response_v2();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

public:
	void set_rx_sample_freq(double sample_freq);

private:
	int get_2g4_nearest_freq(int CF);
	int get_rx_rf_response_data(char *file_buf);
	int get_rx_rf_response_from_buf(char *file_buf, int len, int unit, int type);
	int rx_spectral_flatness_compensate(int unit, double sample_freq, int CF, int port, Rx_Spec_Flat_Comp_Parm *rx_flat_comp_parm);
};

#endif /* INCLUDE_CAL_MODEL_RX_RESPONSE_H_ */
