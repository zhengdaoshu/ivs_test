/*
 * cal_error.h
 *
 *  Created on: 2016年11月25日
 *      Author: J00054
 */

#ifndef INCLUDE_BASE_CAL_ERROR_H_
#define INCLUDE_BASE_CAL_ERROR_H_

/* 错误编号 */
enum err_code
{
	ERR_RET_OK = 0,
	ERR_CAL_NOT_INIT = 1,
	ERR_FREQ_INVALID,
	ERR_OCXO_CSV_LOST,
	ERR_TX_DC_OFFSET_CSV_LOST,	    /* tx_dc_offset.csv文件缺失 */
	ERR_RX_DC_OFFSET_CSV_LOST,	    /* rx_dc_offset.csv文件缺失 */
	ERR_TX_IQ_IMB_CSV_LOST,	        /* tx_iq_imb.csv文件缺失 */
	ERR_RX_IQ_IMB_CSV_LOST,         /* rx_iq_imb.csv文件缺失 */
	ERR_TX_CONST_CSV_LOST,		    /* tx_const_gain.csv文件缺失 */
	ERR_RX_CONST_CSV_LOST,		    /* rx_const_gain.csv文件缺失 */
	ERR_RX_ATT1_CSV_LOST = 10,		/* rx_att_1.csv文件缺失 */
	ERR_RX_ATT2_CSV_LOST,			/* rx_att_2.csv文件缺失 */
	ERR_TX_ATT1_CSV_LOST,			/* tx_att_1.csv文件缺失 */
	ERR_TX_ATT2_CSV_LOST,			/* tx_att_2.csv文件缺失 */
	ERR_TX_ATT3_CSV_LOST,			/* tx_att_3.csv文件缺失 */
	ERR_TX_ATT4_CSV_LOST,			/* tx_att_4.csv文件缺失 */
	ERR_TX_RF_RESPONSE1_CSV_LOST,
	ERR_TX_RF_RESPONSE2_CSV_LOST,
	ERR_TX_RF_RESPONSE3_CSV_LOST,
	ERR_TX_RF_RESPONSE4_CSV_LOST,
	ERR_TX_RF_RESPONSE5_CSV_LOST = 20,
	ERR_TX_RF_RESPONSE6_CSV_LOST,
	ERR_TX_RF_RESPONSE7_CSV_LOST,
	ERR_TX_RF_RESPONSE8_CSV_LOST,
	ERR_TX_BB_RESPONSE1_CSV_LOST,
	ERR_TX_BB_RESPONSE2_CSV_LOST,
	ERR_RX_RF_RESPONSE1_CSV_LOST,
	ERR_RX_RF_RESPONSE2_CSV_LOST,
	ERR_RX_RF_RESPONSE3_CSV_LOST,
	ERR_RX_RF_RESPONSE4_CSV_LOST,
	ERR_RX_RF_RESPONSE5_CSV_LOST = 30,
	ERR_RX_RF_RESPONSE6_CSV_LOST,
	ERR_RX_RF_RESPONSE7_CSV_LOST,
	ERR_RX_RF_RESPONSE8_CSV_LOST,
	ERR_RX_BB_RESPONSE1_CSV_LOST,
	ERR_RX_BB_RESPONSE2_CSV_LOST,
	ERR_BBVGA_CSV_LOST,		        /* bbvga.csv文件缺失 */
	ERR_SW_0202_CSV_LOST,
	ERR_SW_0204_CSV_LOST,
	ERR_CAL_REPORT_LOST,
	ERR_TEMP_COMPENSATION_LOST = 40,/* temp_compensation.csv文件缺失 */
	ERR_TX_COMP_CSV_LOST,
	ERR_RX_COMP_CSV_LOST,
	ERR_RF_CONFIG_CSV_LOST,	        /* rf_config.csv文件缺失 */
	ERR_WT_CONFIG_CSV_LOST,	        /* wt_config.csv文件缺失 */
	ERR_LO_GAIN_LOST,			    /* lo-gain.csv文件缺失 */
	ERR_LO_GAIN_NOT_INIT,
	ERR_SN_NOT_MATCH,
	ERR_TX_BB_GAIN_COUNT,
	ERR_RX_BB_GAIN_COUNT,
	ERR_CAL_CONFIG = 50,
	ERR_CAL_TX_IQ_IMB_NULL,
	ERR_NONE
};

#endif /* INCLUDE_BASE_CAL_ERROR_H_ */
