/*
 * cal_factory.cpp
 *
 *  Created on: 2016Äê12ÔÂ7ÈÕ
 *      Author: J00054
 */
#include<stdlib.h>
#include "../../include/cal_model/cal_factory.h"
#include "../../include/base/typedef.h"
#include "../../include/config/config.h"
#include "../../include/cal_model/ocxo.h"
#include "../../include/cal_model/bbvga.h"
#include "../../include/cal_model/tx_dc_offset.h"
#include "../../include/cal_model/rx_dc_offset.h"
#include "../../include/cal_model/tx_iq_imb.h"
#include "../../include/cal_model/rx_iq_imb.h"
#include "../../include/cal_model/att_vsa.h"
#include "../../include/cal_model/att_vsg.h"
#include "../../include/cal_model/tx_const_gain.h"
#include "../../include/cal_model/rx_const_gain.h"
#include "../../include/cal_model/tx_gain_compensation_port.h"
#include "../../include/cal_model/rx_gain_compensation_port.h"
#include "../../include/cal_model/sw_0202.h"
#include "../../include/cal_model/sw_0204.h"
#include "../../include/cal_model/temp_compensation.h"
#include "../../include/cal_model/tx_response.h"
#include "../../include/cal_model/rx_response.h"

Cal_model* Cal_factory::create_cal_model(int cal_type, int csv_type)
{
	Cal_model* result = NULL;
    switch (cal_type)
    {
        case CalOcxo:
            result = new Ocxo(csv_type);
            break;
        case CalBbvga:
        	result = new Bbvga(csv_type);
        	break;
        case CalTxDcOffset:
            result = new Tx_dc_offset(csv_type);
            break;
        case CalRxDcOffset:
            result = new Rx_dc_offset(csv_type);
            break;
        case CalTxIqImb:
            result = new Tx_iq_imb(csv_type);
            break;
        case CalRxIqImb:
            result = new Rx_iq_imb(csv_type);
            break;
        case CalTxAtt:
            result = new Att_vsg(csv_type);
            break;
        case CalRxAtt:
            result = new Att_vsa(csv_type);
            break;
        case CalTxConstGain:
            result = new Tx_const_gain(csv_type);
            break;
        case CalRxConstGain:
            result = new Rx_const_gain(csv_type);
            break;
        case CalTxCompensationGain:
            result = new Tx_gain_compensation_port(csv_type);
            break;
        case CalRxCompensationGain:
            result = new Rx_gain_compensation_port(csv_type);
            break;
        case CalTxResponse:
            if (Rf_Config.flatness_vsg_version)
            {
                result = new Tx_response_v2(csv_type);
            }
            else
            {
                result = new Tx_response(csv_type);
            }
            break;
        case CalRxResponse:
            if (Rf_Config.flatness_vsa_version)
            {
                result = new Rx_response_v2(csv_type);
            }
            else
            {
                result = new Rx_response(csv_type);
            }
            break;
        case CalSW0202:
            result = new Sw_0202_loss(csv_type);
            break;
        case CalSW0204:
            result = new Sw_0204_loss(csv_type);
            break;
        case CalTEMPERATURE:
			result = new Temp_config(csv_type);
			break;
        default:
        	break;
    }
    return result;
}



