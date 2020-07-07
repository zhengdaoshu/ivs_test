/*
 * calibration_model.h
 *
 *  Created on: 2016年11月25日
 *      Author: J00054
 */
#ifndef INCLUDE_CAL_MODEL_CAL_MODEL_H_
#define INCLUDE_CAL_MODEL_CAL_MODEL_H_
#include "../base/typedef.h"
#include "../csv_file_manager/csv_model.h"

#define TX_DC_OFFSET_MAX_COUNT (3)
#define ATT_CODE_MAX (64)
#define PORT_MAX (9)
#define VSA_PORT_MAX (8)
#define BBVGA_CODE_MAX (64)
#define MAX_CHANNEL_COUNT (6000-400+1)
#define UNIT_NUM (2)
#define MHz (1000000)

#define RESPONSE_CHANNEL_COUNT (6120-4780+1)
#define RESPONSE_CHANNEL_BASE (4780)
#define RESPONSE_CHANNEL_END (6120)
#define RESPONSE_CHANNEL_MID (5500)
#define RESPONSE_2G_CHANNEL_BASE 2280
#define RESPONSE_2G_CHANNEL_END  2620

#define FREQ_2G_NUM  (RESPONSE_2G_CHANNEL_END - RESPONSE_2G_CHANNEL_BASE + 1)
#define FREQ_5G_NUM  (RESPONSE_CHANNEL_END - RESPONSE_CHANNEL_BASE + 1)
#define FREQ_NUM     (FREQ_2G_NUM + FREQ_5G_NUM)
#define FREQ_INDEX(freq)  ((freq) <= RESPONSE_2G_CHANNEL_END ? (freq) - RESPONSE_2G_CHANNEL_BASE \
                                                             : (freq) - RESPONSE_CHANNEL_BASE + FREQ_2G_NUM)

enum CalibrationType
{
	CalOcxo = 0,
	CalBbvga,
	CalTxDcOffset,
	CalRxDcOffset,
	CalTxIqImb,
	CalRxIqImb,
	CalTxAtt,
	CalRxAtt,
	CalTxConstGain,
	CalRxConstGain,
	CalTxCompensationGain,
	CalRxCompensationGain,
	CalTxResponse,
	CalRxResponse,
	CalSW0202,
	CalSW0204,
	CalTEMPERATURE,
	CalEnd
};

#define CAL_MAX_NUM (CalEnd)

class Cal_model
{
private:
	int model_type;

protected:
	int m_src_type;
	Csv_file_manager *p_csv_manager;
	int unit_map[6];

public:
	Cal_model(int csv_type);
	virtual ~Cal_model();

    //*****************************************************************************
    // 函数: set_csv_type()
    // 功能：设置校准目标文件来源类型
    // 参数[IN] : csv_type : 校准目标文件来源
	//            src_disk  = 0, 表示从硬盘中读取校准文件
	//            src_flash = 1, 从芯片模块flash中读取校准文件
    // 返回值: 0
    //*****************************************************************************
	int set_csv_type(int csv_type);

	virtual int wt_calibration_initial(char *buf) = 0;
	virtual double get_calibration_data(char *ref_buf) = 0;
	virtual void calibration_data_reset() = 0;

public:
	int set_calibration_data(char *filename, char *buf, int len);
	int get_calibration_data(char *filename, char *buf, int bufsize);

	int get_unit_no(int unit);

private:
	void init_unit_map();
};

#endif /* INCLUDE_CAL_MODEL_CAL_MODEL_H_ */
