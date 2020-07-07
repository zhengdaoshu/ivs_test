/*
 * tx_const_gain_response.h
 *
 *  Created on: 2016年11月30日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_TX_RESPONSE_H_
#define INCLUDE_CAL_MODEL_TX_RESPONSE_H_
#include "cal_model.h"
#include "../wt-calibration.h"

#define TX_SPAN     (769)
#define TX_SPAN_OLD (241)
//中频个数
#define TX_IF_COUNT (5)
typedef struct _tx_rf_gain
{
	double rf_gain_pa[PORT_MAX][TX_IF_COUNT];  /* 射频功率，8个端口， 5个中频 */
	double rf_gain_pa_off[PORT_MAX][TX_IF_COUNT];
	double ac80_host_pa[PORT_MAX][TX_IF_COUNT]; /* ac80_80主机，放大器打开 */
	double ac80_host_pa_off[PORT_MAX][TX_IF_COUNT];
	double ac80_slave_pa[PORT_MAX][TX_IF_COUNT];
	double ac80_slave_pa_off[PORT_MAX][TX_IF_COUNT];
} T_tx_rf_gain;

typedef struct _tx_bb_gain
{
	double bb_gain[UNIT_NUM][TX_IF_COUNT];  /* 基带功率，2个单元， 5个中频 */
} T_tx_bb_gain;

typedef struct _tx_const_gain_response_data
{
	int freq;	           /* 频率 索引 */
	T_tx_bb_gain tx_bb_gain;
	T_tx_rf_gain tx_rf_gain;
} T_tx_response_data;

class Tx_response :public Cal_model
{
private:
	T_tx_response_data * m_cal_data;
	double m_tx_gain;
	double m_sample_freq; //采样率
	int data_flag;   //支持ac_80_80平坦度校准数据

public:
	Tx_response(int csv_type);
	virtual ~Tx_response();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

public:
	double get_tx_const_gain();
	void set_tx_sample_freq(double sample_freq);

private:
	int init_freq();
	int fill_cal_data(int unit);
	int get_tx_rf_response_data(char *file_buf);
	int get_tx_bb_response_data(char *file_buf);
	int get_tx_rf_response_from_buf(char *file_buf, int len, int unit);
	int get_tx_bb_response_from_buf(char *file_buf, int len, int unit);

	int tx_spectral_flatness_compensate(int unit, int CF, int port, int is_pa_on, Tx_Spec_Flat_Comp_Parm *tx_flat_comp_parm);
};


struct tx_response_item
{
	double gain_pa[TX_SPAN];
	double gain_pa_off[TX_SPAN];
	double host_gain_pa[TX_SPAN];
	double host_gain_pa_off[TX_SPAN];
	double slave_gain_pa[TX_SPAN];
	double slave_gain_pa_off[TX_SPAN];
};

class Tx_response_v2 : public Cal_model
{
private:
	tx_response_item *m_cal_data[PORT_MAX];
	double m_tx_gain;
	double m_sample_freq; //采样率
	int data_flag;   //支持ac_80_80平坦度校准数据

public:
	Tx_response_v2(int csv_type);
	virtual ~Tx_response_v2();

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

public:
	double get_tx_const_gain();
	void set_tx_sample_freq(double sample_freq);

private:
	int get_tx_rf_response_data(char *file_buf);
	int get_tx_rf_response_from_buf(char *file_buf, int len, int unit, int type);
	int tx_spectral_flatness_compensate(int unit, double sample_freq, int CF, int port, int is_pa_on, Tx_Spec_Flat_Comp_Parm *tx_flat_comp_parm);
	int get_2g4_nearest_freq(int CF);
};

#endif /* INCLUDE_CAL_MODEL_TX_RESPONSE_H_ */
