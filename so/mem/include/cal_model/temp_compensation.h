/*
 * temp_compensation.h
 *
 *  Created on: 2016年12月15日
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_TEMP_COMPENSATION_H_
#define INCLUDE_CAL_MODEL_TEMP_COMPENSATION_H_
#include "cal_model.h"

#define TEMP_COLOUM (9)
#define TX_TEMP_COLOUM (6)
#define RX_TEMP_COLOUM (3)
//温度范围个数
#define TEMP_BAND_COUNT (4)
enum TEMP_COMPENSATION_TYPE
{
	siso_tx_pa_on = 0,
	siso_tx_pa_off,
	ac80_master_tx_pa_on,
	ac80_master_tx_pa_off,
	ac80_slave_tx_pa_on,
	ac80_slave_tx_pa_off,
	siso_rx,                     //siso 主机 rx
	ac80_master_rx,              //ac80+80  主机 rx
	ac80_slave_rx,               //ac80+80  从机 rx
};

typedef struct _temp_compensation_data
{
	int freq;	    /* 频率 索引 */
	int reserve;
	BOOL tx_flag[TEMP_BAND_COUNT][PORT_MAX];      /* 校准数据填充标记 */
	BOOL rx_flag[TEMP_BAND_COUNT][PORT_MAX];      /* 校准数据填充标记 */
	BOOL sw_tx_flag[TEMP_BAND_COUNT][PORT_MAX];      /* 校准数据填充标记 */
	BOOL sw_rx_flag[TEMP_BAND_COUNT][PORT_MAX];      /* 校准数据填充标记 */
	double temp_comp[TEMP_BAND_COUNT][PORT_MAX][TEMP_COLOUM];  //射频温补系数
	double sw_temp_comp[TEMP_BAND_COUNT][PORT_MAX][2];  //SW开关板温补系数
} T_temp_config;

class Temp_config : public Cal_model
{
private:
	T_temp_config* m_temp_data;

	BOOL temp_flag; // 0使用单元温度， 1 使用端口温度

	/*校准温度*/
	double cal_rx_temp[UNIT_NUM];
	double cal_tx_temp[UNIT_NUM];
	double tx_port_temp[PORT_MAX];
	double rx_port_temp[PORT_MAX];

	double tx_sw_port_temp[PORT_MAX];
	double rx_sw_port_temp[PORT_MAX];
	char cal_report_ver[32];
public:
	Temp_config(int csv_type);
	virtual ~Temp_config();

	virtual int wt_calibration_initial(char *buf);
	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();

	double get_tx_compensation(double tx_gain, char *ref_buf);
	double get_rx_compensation(double rx_gain, char *ref_buf);

private:
	int init_freq();
	void init_temp_variable();
	int get_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port);
	int get_tx_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port);
	int get_rx_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port);
	int get_tx_sw_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port);
	int get_rx_sw_temp_compensation_from_buf(char *file_buf, int len, int temp_band, int port);
	int get_cal_temp_from_buf(char *file_buf, int len);
	int fill_tx_cal_data(int temp_band, int port);
	int fill_rx_cal_data(int temp_band, int port);

	int fill_tx_sw_cal_data(int temp_band, int port);
	int fill_rx_sw_cal_data(int temp_band, int port);

	int fill_cal_data(int temp_band, int port);

	char* gets_buff_line(char* buff, u32 buff_len, char** ppBuff);
	int get_csv_line_value(const char *keyword, char *buf, double *value);

	int get_temp_subband_mode(double rf_temperture);
	int get_temp_conlum_type(int mode, int unit, int is_pa_on);

	int show_sw_cal_data(int temp_band, int port);
	int show_cal_data(int temp_band, int port);
	int show_sw_cal_flag(int temp_band, int port);

	int get_cal_report_version_from_buf(char *file_buf, int len);
	void reset_dac_margin_value();
	void check_response_version();
};



#endif /* INCLUDE_CAL_MODEL_TEMP_COMPENSATION_H_ */
