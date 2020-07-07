/*
 * cal_data_manager.cpp
 *
 *  Created on: 2016Äê11ÔÂ24ÈÕ
 *      Author: J00054
 */
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include <sys/time.h>
#include "../include/base/basefun.h"
#include "../include/cal_data_manager.h"
#include "../include/csv_file_manager/csv_model.h"
#include "../include/cal_model/cal_factory.h"
#include "../include/base/app_lib.h"
#include "../include/hal/rf_set.h"
#include "../include/hal/cal_data.h"
Cal_data_manager::Cal_data_manager()
{
	m_cal_src_type = src_disk;
	m_wtconfig = new WTConfig();
	wt_config_initial();
	init_cal_modeles();
	rf_power = new Rf_power(m_cal_model);
}

Cal_data_manager::~Cal_data_manager()
{
	int i;
	if(m_wtconfig != NULL)
	{
		delete m_wtconfig;
		m_wtconfig = NULL;
	}
	for(i=CalOcxo; i<CalEnd; i++)
	{
		if(m_cal_model[i] != NULL)
		{
		    delete m_cal_model[i];
		    m_cal_model[i] = NULL;
		}
	}
	if(rf_power != NULL)
	{
		delete rf_power;
		rf_power = NULL;
	}
}

int Cal_data_manager::wt_config_initial()
{
	int ret = ERR_RET_OK;
	char *file_buf = (char *)App_lib::app_malloc(CAL_FILE_SIZE);
	ret = m_wtconfig->wt_configuration_initial(file_buf);
	App_lib::app_free(file_buf);
	printf("wt_config_initial,ret=%d\n",ret);
	return ret;
}

int Cal_data_manager::init_cal_modeles()
{
	int i;
	for(i=CalOcxo; i<CalEnd; i++)
	{
		m_cal_model[i] = Cal_factory::create_cal_model(i, m_cal_src_type);
	}
	return 0;
}

const  char* cal_name[]={
	"CalOcxo",
	"CalBbvga",
	"CalTxDcOffset",
	"CalRxDcOffset",
	"CalTxIqImb",
	"CalRxIqImb",
	"CalTxAtt",
	"CalRxAtt",
	"CalTxConstGain",
	"CalRxConstGain",
	"CalTxCompensationGain",
	"CalRxCompensationGain",
	"CalTxResponse",
	"CalRxResponse",
	"CalSW0202",
	"CalSW0204",
	"CalTEMPERATURE",
 };

static long long int tvsub(struct timeval *t1, struct timeval *t2)
{
    if(t1->tv_usec > t2->tv_usec)
    {
        return (t1->tv_sec - t2->tv_sec) * 1000000 + (t1->tv_usec - t2->tv_usec);
    }
    else
    {
        return (t1->tv_sec - t2->tv_sec - 1) * 1000000 + (t1->tv_usec + 1000000 - t2->tv_usec);
    }
}

static void *start_routine(void *args)
{
//    struct timeval start, finish;
//	long long int difftime = 0;
	char *file_buf = (char *)App_lib::app_malloc(CAL_FILE_SIZE);
	Cal_model* model = (Cal_model*)args;

//	gettimeofday(&start, NULL);
	model->wt_calibration_initial(file_buf);

//    gettimeofday(&finish, NULL);
//	difftime = tvsub(&finish, &start);
//	printf("[%s,%d] use time:%lld us, %lldms\n", __FUNCTION__,__LINE__,difftime, difftime/1000);
	App_lib::app_free(file_buf);
	return nullptr;
}

int Cal_data_manager::wt_calibration_initial()
{
	//int ret = ERR_RET_OK;
	int i_ret = ERR_RET_OK;
	int i = 0;
    struct timeval start, finish;
	long long int difftime = 0;
	char *file_buf = (char *)App_lib::app_malloc(CAL_FILE_SIZE);
	gettimeofday(&start, NULL);
	do
	{
		i_ret = m_wtconfig->wt_configuration_initial(file_buf);
		err_break(i_ret);

		for(i = CalOcxo; i < CalEnd; i++)
		{
			m_cal_model[i]->calibration_data_reset();
		}

		pthread_t handle[CalEnd];
		for(i = CalEnd-1; i >= CalOcxo; i--)
		{
			if(nullptr != m_cal_model[i])
			{
				pthread_create(&handle[i], nullptr, start_routine, m_cal_model[i]);
			}
//			gettimeofday(&start, NULL);
//			ret = m_cal_model[i]->wt_calibration_initial(file_buf);
//			printf("ret=%d,i=%d,%s\n",ret,i,cal_name[i]);
//		    gettimeofday(&finish, NULL);
//			difftime = tvsub(&finish, &start);
//			printf("[%s,%d] use time:%lld us, %lldms\n", __FUNCTION__,__LINE__,difftime, difftime/1000);
//			if(ret != ERR_RET_OK)
//			{
//				i_ret = ret;
//			}
			//err_break(ret);
		}
		for(i = CalOcxo; i < CalEnd; i++)
		{
			if(nullptr != m_cal_model[i])
			{
				pthread_join(handle[i], nullptr);
			}
		}
	}while(0);

	App_lib::app_free(file_buf);
//	int cal_total = (sizeof(cal_name)/sizeof(cal_name[0]));
//	if(i < cal_total)
//	{
//		printf("wt_calibration_initial,ret=%d,err_cal_step=%s\n",ret,cal_name[i]);
//	}
//	else
//	{
//		printf("wt_calibration_initial,ret=%d\n",ret);
//	}
	printf("wt_calibration_initial,ret=%d\n",i_ret);
	gettimeofday(&finish, NULL);
	difftime = tvsub(&finish, &start);
	printf("[%s,%d] use time:%lld us, %lldms\n", __FUNCTION__,__LINE__,difftime, difftime/1000);
	return i_ret;
}

int Cal_data_manager::get_ocxo_code(Ocxo_Parm *ocxo_parm)
{
	m_cal_model[CalOcxo]->get_calibration_data((char*)ocxo_parm);
	return 0;
}

int Cal_data_manager::get_tx_setting(Tx_Parm *tx_parm, double sample_freq)
{
	int ret = ERR_RET_OK;
	do
	{
		printf("get_tx_setting sample_freq=%lf\n",sample_freq);
		ret = Rf_set::is_valid_freq(tx_parm->freq);
		err_break(ret);
		ret = Rf_set::get_tx_lo_setting(tx_parm->unit, tx_parm->freq, &(tx_parm->freq_parm));
		err_break(ret);
		m_cal_model[CalTxIqImb]->get_calibration_data((char*)tx_parm);
		rf_power->get_tx_gain_setting(tx_parm, sample_freq);
		m_cal_model[CalTxDcOffset]->get_calibration_data((char*)tx_parm);
	}while(0);
	return ret;
}

int Cal_data_manager::get_rx_setting(Rx_Parm *rx_parm, double sample_freq)
{
	int ret = ERR_RET_OK;
	do
	{
		ret = Rf_set::is_valid_freq(rx_parm->freq);
		err_break(ret);
		ret = Rf_set::get_rx_lo_setting(rx_parm->unit, rx_parm->freq, &(rx_parm->freq_parm));
		err_break(ret);
		m_cal_model[CalRxIqImb]->get_calibration_data((char*)rx_parm);
		m_cal_model[CalRxDcOffset]->get_calibration_data((char*)rx_parm);
		rf_power->get_rx_gain_setting(rx_parm, sample_freq);
	}while(0);
	return ret;
}

int Cal_data_manager::get_configuration(Rf_Config_Parm *rf_config_parm)
{
	return m_wtconfig->get_rf_configuration(rf_config_parm);
}

int Cal_data_manager::set_calibration_option(int buf_size, void *cal_opt)
{
	return m_wtconfig->set_calibration_option(buf_size, cal_opt);
}

int Cal_data_manager::get_calibration_option(int len, void *cal_opt)
{
	return m_wtconfig->get_calibration_option(len, cal_opt);
}

int Cal_data_manager::set_calibration_data(char *filename, char *buf, int len)
{
	return m_cal_model[CalOcxo]->set_calibration_data(filename, buf, len);
}

int Cal_data_manager::get_calibration_data(char *filename, char *buf, int size)
{
	return m_cal_model[CalOcxo]->get_calibration_data(filename, buf, size);
}

int Cal_data_manager::set_configuration_data(char *filename, char *buf, int len)
{
	return m_wtconfig->set_configuration_data(filename, buf, len);
}

int Cal_data_manager::get_configuration_data(char *filename, char *buf, int size)
{
	return m_wtconfig->get_configuration_data(filename, buf, size);
}
