/*******************************************************************************
*
* file name: wt-calibration.h
* descript:
* create by: liweidong 2016.10.17
*
*******************************************************************************/
#ifndef __WT_CALIBRATIION_H__
#define __WT_CALIBRATIION_H__

#ifdef __cplusplus
extern "C" {
#define WT_CALIBRATION_EXTERN extern "C"
#endif

/// 内部参考时钟OCXO参数
typedef struct OCXO_Parameters
{
	int ocxo_code1;
	int ocxo_code2;
} Ocxo_Parm;

typedef struct RF_Configuration_Parameters
{
	double dac_margin;
	double cal_port_temp_avg;
} Rf_Config_Parm;

/// TX DC Offset参数
typedef struct TX_DC_Offset_Parameters
{
	int i_code;
	int q_code;
} Tx_DC_Offset_Parm;

/// RX DC Offset参数
typedef struct RX_DC_Offset_Parameters
{
	int i_code;
	int q_code;
} Rx_DC_Offset_Parm;

/// TX I/Q 不平衡参数
typedef struct TX_IQ_Imbalance_Parameters
{
	double gain_imb;
	double quad_err;
	double timeskew;
} Tx_Iq_Imb_Parm;

/// RX I/Q 不平衡参数
typedef struct RX_IQ_Imbalance_Parameters
{
	double gain_imb;
	double quad_err;
	double timeskew;
} Rx_Iq_Imb_Parm;

/// TX链路ATT器件数量
#define TX_ATT_COUNT (4)
/// TX链路功率配置参数
typedef struct TX_Gain_Parameters
{
	bool is_pa_on;	// false(0)表示放大器关闭，true(1)表示放大器开启
	int att_code[TX_ATT_COUNT];
	double dac_gain;
	double actual_mpl; /* General RF TX gain in dB 射频链路衰减的实际功率 */
} Tx_Gain_Parm;

/// RX链路ATT器件数量
#define RX_ATT_COUNT (2)
/// RX链路功率配置参数
typedef struct RX_Gain_Parameters
{
	int att_code[RX_ATT_COUNT];
	double final_gain;  /*实际的链路增益*/
	double actual_mpl;
} Rx_Gain_Parm;

/// RX链路功率配置参数
typedef struct RX_Trigger_Parameters
{
	int dac_code;
} Rx_Trig_Parm;

/// 不同的采样带宽所对应的数量也不同
#define TX_SPECTRUM_SUBCHAN_COUNT (384 * 2 + 1)
/// TX频谱平坦度补偿参数
typedef struct TX_Spectrum_Flatness_Compensate_Parameters
{
	int bb_comp_len;
	int rf_comp_len;
	double bb_comp_gain[TX_SPECTRUM_SUBCHAN_COUNT];
	double rf_comp_gain[TX_SPECTRUM_SUBCHAN_COUNT];
} Tx_Spec_Flat_Comp_Parm;

/// 不同的采样带宽所对应的数量也不同
/// 240/0.3125 = 768
#define RX_SPECTRUM_SUBCHAN_COUNT (384 * 2 + 1)
/// TX频谱平坦度补偿参数
typedef struct RX_Spectrum_Flatness_Compensate_Parameters
{
	int bb_comp_len;
	int rf_comp_len;
	double bb_comp_gain[RX_SPECTRUM_SUBCHAN_COUNT];
	double rf_comp_gain[RX_SPECTRUM_SUBCHAN_COUNT];
} Rx_Spec_Flat_Comp_Parm;

/// TX链路本振器件数量
#define LO_COUNT (2)

/// 本振配置参数
typedef struct LO_Parameters
{
	int lo;			// 表示哪一个本振
	double freq;
	int power_level;	// HMC833为0~3  767待定
} Lo_Parm;

/// 频率配置参数
typedef struct Freq_Parameters
{
    Lo_Parm LoParm[LO_COUNT];
    int BandMode;
} Freq_Parm;

/// TX链路综合参数
typedef struct TX_Parameters
{
	int unit;		// 单元编号，表示使用哪一个单元
	int rf_port;	// 射频端口
	double freq;	// 载波频率，单位为Hz
	double power;	// 输出电平，单位为dBm
	double temperature;	// 板上温度，单位为摄氏度
	double sw_temperature;	// 开关板上温度，单位为摄氏度

	Freq_Parm freq_parm;
	Tx_DC_Offset_Parm tx_dc_offset_parm;
	Tx_Iq_Imb_Parm tx_iq_imb_parm;
	Tx_Spec_Flat_Comp_Parm tx_spec_flat_comp_parm;
	Tx_Gain_Parm tx_gain_parm;
} Tx_Parm;

/// TX链路传入参数
typedef struct TX_Handles
{
	int unit;		// 单元编号，表示使用哪一个单元
	int rf_port;	// 射频端口
	double freq;	// 载波频率，单位为Hz
	double power;	// 输出电平，单位为dBm
	double temperature;	// 板上温度，单位为摄氏度
	double dac_gain;
} TX_Handle;

/// RX链路综合参数
typedef struct RX_Parameters
{
	int unit;			  // 单元编号，表示使用哪一个单元
	int rf_port;		  // 射频端口
	double freq;		  // 载波频率，单位为Hz
	double ref_power;	  // 参考电平，单位为dBm
	bool pd_enable;		  // 启用功率检测trigger模式
	double trigger_level;	// 触发电平
	double temperature;		// 板上温度，单位为摄氏度
	double sw_temperature;	// 开关板上温度，单位为摄氏度

	Freq_Parm freq_parm;
	Rx_DC_Offset_Parm rx_dc_offset_parm;
	Rx_Iq_Imb_Parm rx_iq_imb_parm;
	Rx_Spec_Flat_Comp_Parm rx_spec_flat_comp_parm;
	Rx_Gain_Parm rx_gain_parm;
	Rx_Trig_Parm rx_trig_parm;
} Rx_Parm;

/// RX链路传入参数
typedef struct RX_Handles
{
	int unit;			// 单元编号，表示使用哪一个单元
	int rf_port;		// 射频端口
	double freq;		// 载波频率，单位为Hz
	double ref_power;	// 参考电平，单位为dBm
	bool pd_enable;		// 启用功率检测trigger模式
	double trigger_level;	// 触发电平
	double temperature;		// 板上温度，单位为摄氏度
} RX_Handle;


/////////////////////////////////////////////////////////////////////////////////
//
// 以下是正常业务时调用的接口
//
////////////////////////////////////////////////////////////////////////////////

/// 校准模块版本号
WT_CALIBRATION_EXTERN const char* wt_calibration_version();

/// 校准模块初始化
WT_CALIBRATION_EXTERN int wt_calibration_initial();

/// 与校准相关的器件初始化配置：本振、ADC、DAC、DAC Margin、ADC期望值等
WT_CALIBRATION_EXTERN int wt_calibration_get_configuration(Rf_Config_Parm *rf_config_parm);

/// 系统初始化时获取OCXO code并设置AD5611
WT_CALIBRATION_EXTERN int wt_calibration_get_ocxo_setting(Ocxo_Parm *ocxo_parm);

/// 获取TX链路配置参数
WT_CALIBRATION_EXTERN int wt_calibration_get_tx_setting(Tx_Parm *tx_parm, double sample_freq);

/// 获取RX链路配置参数
WT_CALIBRATION_EXTERN int wt_calibration_get_rx_setting(Rx_Parm *rx_parm, double sample_freq);

////////////////////////////////////////////////////////////////////////////////
//
// 以下是校准和调试时调用的接口
//
////////////////////////////////////////////////////////////////////////////////
/// 从仪器获取校准数据（文件）
WT_CALIBRATION_EXTERN int wt_calibration_get_calibration_data(char *filename, char *buf, int bufsize, int len);

/// 向仪器写入校准数据（文件）
WT_CALIBRATION_EXTERN int wt_calibration_set_calibration_data(char *filename, char *buf, int len);

/// 从仪器获取校准配置数据（文件）
WT_CALIBRATION_EXTERN int wt_calibration_get_calibration_configuration_data(char *filename, char *buf, int bufsize, int len);

/// 向仪器写入校准配置数据（文件）
WT_CALIBRATION_EXTERN int wt_calibration_set_calibration_configuration_data(char *filename, char *buf, int len);

/// 使能开关
typedef struct Calibration_Option
{
	int unit;		            // 单元编号，表示使用哪一个单元
	int unit_type;				//单元板类型
	int tx_temp_comp_enable;	// TX温补使能，false(0)表示不启用温补，true(1)表示启用
	int rx_temp_comp_enable;	// RX温补使能，false(0)表示不启用温补，true(1)表示启用
	int tx_iq_imb_enable;		// TX I/Q不平衡补偿使能，false(0)表示不启用补偿，true(1)表示启用
	int rx_iq_imb_enable;		// RX I/Q不平衡补偿使能，false(0)表示不启用补偿，true(1)表示启用
	int tx_spec_flat_enable;	// TX频谱平坦度补偿使能，false(0)表示不启用补偿，true(1)表示启用
	int rx_spec_flat_enable;	// RX频谱平坦度补偿使能，false(0)表示不启用补偿，true(1)表示启用
	double tx_pa_threshold_5g;
	double tx_pa_threshold_2g;
	int calibration_flat;       // 校准模式开关，false(0)表示正常工作模式，true(1)表示校准模式
	int reserve;
} Cal_Opt;
/// 设置使能开关，包括温补、I/Q不平衡补偿、频谱平坦度补偿
WT_CALIBRATION_EXTERN int wt_calibration_set_calibration_option(int buf_size, void *cal_opt);
/// 获取使能开关，包括温补、I/Q不平衡补偿、频谱平坦度补偿
WT_CALIBRATION_EXTERN int wt_calibration_get_calibration_option(int len, void *cal_opt);

#ifdef __cplusplus
}
#endif

#endif /* WT_CALIBRATIION_H */

