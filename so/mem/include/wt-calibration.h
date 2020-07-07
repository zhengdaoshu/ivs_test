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

/// �ڲ��ο�ʱ��OCXO����
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

/// TX DC Offset����
typedef struct TX_DC_Offset_Parameters
{
	int i_code;
	int q_code;
} Tx_DC_Offset_Parm;

/// RX DC Offset����
typedef struct RX_DC_Offset_Parameters
{
	int i_code;
	int q_code;
} Rx_DC_Offset_Parm;

/// TX I/Q ��ƽ�����
typedef struct TX_IQ_Imbalance_Parameters
{
	double gain_imb;
	double quad_err;
	double timeskew;
} Tx_Iq_Imb_Parm;

/// RX I/Q ��ƽ�����
typedef struct RX_IQ_Imbalance_Parameters
{
	double gain_imb;
	double quad_err;
	double timeskew;
} Rx_Iq_Imb_Parm;

/// TX��·ATT��������
#define TX_ATT_COUNT (4)
/// TX��·�������ò���
typedef struct TX_Gain_Parameters
{
	bool is_pa_on;	// false(0)��ʾ�Ŵ����رգ�true(1)��ʾ�Ŵ�������
	int att_code[TX_ATT_COUNT];
	double dac_gain;
	double actual_mpl; /* General RF TX gain in dB ��Ƶ��·˥����ʵ�ʹ��� */
} Tx_Gain_Parm;

/// RX��·ATT��������
#define RX_ATT_COUNT (2)
/// RX��·�������ò���
typedef struct RX_Gain_Parameters
{
	int att_code[RX_ATT_COUNT];
	double final_gain;  /*ʵ�ʵ���·����*/
	double actual_mpl;
} Rx_Gain_Parm;

/// RX��·�������ò���
typedef struct RX_Trigger_Parameters
{
	int dac_code;
} Rx_Trig_Parm;

/// ��ͬ�Ĳ�����������Ӧ������Ҳ��ͬ
#define TX_SPECTRUM_SUBCHAN_COUNT (384 * 2 + 1)
/// TXƵ��ƽ̹�Ȳ�������
typedef struct TX_Spectrum_Flatness_Compensate_Parameters
{
	int bb_comp_len;
	int rf_comp_len;
	double bb_comp_gain[TX_SPECTRUM_SUBCHAN_COUNT];
	double rf_comp_gain[TX_SPECTRUM_SUBCHAN_COUNT];
} Tx_Spec_Flat_Comp_Parm;

/// ��ͬ�Ĳ�����������Ӧ������Ҳ��ͬ
/// 240/0.3125 = 768
#define RX_SPECTRUM_SUBCHAN_COUNT (384 * 2 + 1)
/// TXƵ��ƽ̹�Ȳ�������
typedef struct RX_Spectrum_Flatness_Compensate_Parameters
{
	int bb_comp_len;
	int rf_comp_len;
	double bb_comp_gain[RX_SPECTRUM_SUBCHAN_COUNT];
	double rf_comp_gain[RX_SPECTRUM_SUBCHAN_COUNT];
} Rx_Spec_Flat_Comp_Parm;

/// TX��·������������
#define LO_COUNT (2)

/// �������ò���
typedef struct LO_Parameters
{
	int lo;			// ��ʾ��һ������
	double freq;
	int power_level;	// HMC833Ϊ0~3  767����
} Lo_Parm;

/// Ƶ�����ò���
typedef struct Freq_Parameters
{
    Lo_Parm LoParm[LO_COUNT];
    int BandMode;
} Freq_Parm;

/// TX��·�ۺϲ���
typedef struct TX_Parameters
{
	int unit;		// ��Ԫ��ţ���ʾʹ����һ����Ԫ
	int rf_port;	// ��Ƶ�˿�
	double freq;	// �ز�Ƶ�ʣ���λΪHz
	double power;	// �����ƽ����λΪdBm
	double temperature;	// �����¶ȣ���λΪ���϶�
	double sw_temperature;	// ���ذ����¶ȣ���λΪ���϶�

	Freq_Parm freq_parm;
	Tx_DC_Offset_Parm tx_dc_offset_parm;
	Tx_Iq_Imb_Parm tx_iq_imb_parm;
	Tx_Spec_Flat_Comp_Parm tx_spec_flat_comp_parm;
	Tx_Gain_Parm tx_gain_parm;
} Tx_Parm;

/// TX��·�������
typedef struct TX_Handles
{
	int unit;		// ��Ԫ��ţ���ʾʹ����һ����Ԫ
	int rf_port;	// ��Ƶ�˿�
	double freq;	// �ز�Ƶ�ʣ���λΪHz
	double power;	// �����ƽ����λΪdBm
	double temperature;	// �����¶ȣ���λΪ���϶�
	double dac_gain;
} TX_Handle;

/// RX��·�ۺϲ���
typedef struct RX_Parameters
{
	int unit;			  // ��Ԫ��ţ���ʾʹ����һ����Ԫ
	int rf_port;		  // ��Ƶ�˿�
	double freq;		  // �ز�Ƶ�ʣ���λΪHz
	double ref_power;	  // �ο���ƽ����λΪdBm
	bool pd_enable;		  // ���ù��ʼ��triggerģʽ
	double trigger_level;	// ������ƽ
	double temperature;		// �����¶ȣ���λΪ���϶�
	double sw_temperature;	// ���ذ����¶ȣ���λΪ���϶�

	Freq_Parm freq_parm;
	Rx_DC_Offset_Parm rx_dc_offset_parm;
	Rx_Iq_Imb_Parm rx_iq_imb_parm;
	Rx_Spec_Flat_Comp_Parm rx_spec_flat_comp_parm;
	Rx_Gain_Parm rx_gain_parm;
	Rx_Trig_Parm rx_trig_parm;
} Rx_Parm;

/// RX��·�������
typedef struct RX_Handles
{
	int unit;			// ��Ԫ��ţ���ʾʹ����һ����Ԫ
	int rf_port;		// ��Ƶ�˿�
	double freq;		// �ز�Ƶ�ʣ���λΪHz
	double ref_power;	// �ο���ƽ����λΪdBm
	bool pd_enable;		// ���ù��ʼ��triggerģʽ
	double trigger_level;	// ������ƽ
	double temperature;		// �����¶ȣ���λΪ���϶�
} RX_Handle;


/////////////////////////////////////////////////////////////////////////////////
//
// ����������ҵ��ʱ���õĽӿ�
//
////////////////////////////////////////////////////////////////////////////////

/// У׼ģ��汾��
WT_CALIBRATION_EXTERN const char* wt_calibration_version();

/// У׼ģ���ʼ��
WT_CALIBRATION_EXTERN int wt_calibration_initial();

/// ��У׼��ص�������ʼ�����ã�����ADC��DAC��DAC Margin��ADC����ֵ��
WT_CALIBRATION_EXTERN int wt_calibration_get_configuration(Rf_Config_Parm *rf_config_parm);

/// ϵͳ��ʼ��ʱ��ȡOCXO code������AD5611
WT_CALIBRATION_EXTERN int wt_calibration_get_ocxo_setting(Ocxo_Parm *ocxo_parm);

/// ��ȡTX��·���ò���
WT_CALIBRATION_EXTERN int wt_calibration_get_tx_setting(Tx_Parm *tx_parm, double sample_freq);

/// ��ȡRX��·���ò���
WT_CALIBRATION_EXTERN int wt_calibration_get_rx_setting(Rx_Parm *rx_parm, double sample_freq);

////////////////////////////////////////////////////////////////////////////////
//
// ������У׼�͵���ʱ���õĽӿ�
//
////////////////////////////////////////////////////////////////////////////////
/// ��������ȡУ׼���ݣ��ļ���
WT_CALIBRATION_EXTERN int wt_calibration_get_calibration_data(char *filename, char *buf, int bufsize, int len);

/// ������д��У׼���ݣ��ļ���
WT_CALIBRATION_EXTERN int wt_calibration_set_calibration_data(char *filename, char *buf, int len);

/// ��������ȡУ׼�������ݣ��ļ���
WT_CALIBRATION_EXTERN int wt_calibration_get_calibration_configuration_data(char *filename, char *buf, int bufsize, int len);

/// ������д��У׼�������ݣ��ļ���
WT_CALIBRATION_EXTERN int wt_calibration_set_calibration_configuration_data(char *filename, char *buf, int len);

/// ʹ�ܿ���
typedef struct Calibration_Option
{
	int unit;		            // ��Ԫ��ţ���ʾʹ����һ����Ԫ
	int unit_type;				//��Ԫ������
	int tx_temp_comp_enable;	// TX�²�ʹ�ܣ�false(0)��ʾ�������²���true(1)��ʾ����
	int rx_temp_comp_enable;	// RX�²�ʹ�ܣ�false(0)��ʾ�������²���true(1)��ʾ����
	int tx_iq_imb_enable;		// TX I/Q��ƽ�ⲹ��ʹ�ܣ�false(0)��ʾ�����ò�����true(1)��ʾ����
	int rx_iq_imb_enable;		// RX I/Q��ƽ�ⲹ��ʹ�ܣ�false(0)��ʾ�����ò�����true(1)��ʾ����
	int tx_spec_flat_enable;	// TXƵ��ƽ̹�Ȳ���ʹ�ܣ�false(0)��ʾ�����ò�����true(1)��ʾ����
	int rx_spec_flat_enable;	// RXƵ��ƽ̹�Ȳ���ʹ�ܣ�false(0)��ʾ�����ò�����true(1)��ʾ����
	double tx_pa_threshold_5g;
	double tx_pa_threshold_2g;
	int calibration_flat;       // У׼ģʽ���أ�false(0)��ʾ��������ģʽ��true(1)��ʾУ׼ģʽ
	int reserve;
} Cal_Opt;
/// ����ʹ�ܿ��أ������²���I/Q��ƽ�ⲹ����Ƶ��ƽ̹�Ȳ���
WT_CALIBRATION_EXTERN int wt_calibration_set_calibration_option(int buf_size, void *cal_opt);
/// ��ȡʹ�ܿ��أ������²���I/Q��ƽ�ⲹ����Ƶ��ƽ̹�Ȳ���
WT_CALIBRATION_EXTERN int wt_calibration_get_calibration_option(int len, void *cal_opt);

#ifdef __cplusplus
}
#endif

#endif /* WT_CALIBRATIION_H */

