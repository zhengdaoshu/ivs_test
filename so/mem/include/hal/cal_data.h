/*
 * cal_data.h
 *
 *  Created on: 2016年11月25日
 *      Author: J00054
 */

#ifndef SOURCE_HAL_CAL_DATA_H_
#define SOURCE_HAL_CAL_DATA_H_


/* ATT器件编号枚举 */
enum VSA_ATT_ENUM
{
	VSA_ATT_NO1,
	VSA_ATT_NO2,
	VSA_ATT_END
};

enum VSG_ATT_ENUM
{
	VSG_ATT_NO1,
	VSG_ATT_NO2,
	VSG_ATT_NO3,
	VSG_ATT_NO4,
	VSG_ATT_END
};

/* 配置类型枚举 */
enum TX_RX_CONFIG_TYPE
{
	CONFIG_TYPE_RX = 0,
	CONFIG_TYPE_TX,
};

//RF端口定义
enum WT_SB_RF_PORT_E
{
    WT_SB_PORT_OFF,                //SB RF port off
    WT_SB_PORT_RF1,                //SB RF port 1
    WT_SB_PORT_RF2,                //SB RF port 2
    WT_SB_PORT_RF3,                //SB RF port 3
    WT_SB_PORT_RF4,                //SB RF port 4
    WT_SB_PORT_RF5,                //SB RF port 5
    WT_SB_PORT_RF6,                //SB RF port 6
    WT_SB_PORT_RF7,                //SB RF port 7
    WT_SB_PORT_RF8                 //SB RF port 8
};

//本振器件ID编号枚举类型
enum LO_ID_E
{
    LO_HMC833,                  //对应LO1通道
    LO_HMC767                   //对应LO2通道
};

enum WT_RF_BAND_MODE
{
    WT_RF_BAND_1G,             // 400M-1G
    WT_RF_BAND_2G,             // 1G-2.4G
    WT_RF_BAND_2_4G,           // 2.4G-2.5G
    WT_RF_BAND_3G,             // 2.5G-3.8G
    WT_RF_BAND_4G,             // 3.8G-4.9G
    WT_RF_5G_SUB_BAND_1,       //VSA:[4900,5060]MHz  VSG:[4900,5215]MHz
    WT_RF_5G_SUB_BAND_2,       //VSA:[5060,5450]MHz  VSG:[5215,5450]MHz
    WT_RF_5G_SUB_BAND_3,       //VSA:[5450,5685]MHz  VSG:[5450,5685]MHz
    WT_RF_5G_SUB_BAND_4,       //VSA:[5685,6000]MHz  VSG:[5685,6000]MHz
    WT_RF_MAX_BAND
};

//中频类型
enum IF_5G_MODE
{
	IF_5G_1 = 0,
	IF_5G_2,
	IF_5G_3,
	IF_5G_4,
    IF_5G_5,
    IF_5G_6,
	IF_5G_END
};

enum TEMPERATURE_SUBBAND
{
	Temp_band1 = 0,
	Temp_band2,
	Temp_band3,
	Temp_band4,
	Temp_band_END
};

enum UNIT_NO_TYPE
{
	SISO_UNIT1 = 0,                //单元1校准数据
	SISO_UNIT2,                    //单元2校准数据
	UNIT1_MASTER_UNIT2_SLAVE1,     //单元1校准数据
	UNIT1_MASTER_UNIT2_SLAVE2,     //单元2校准数据
	UNIT1_SLAVE_UNIT2_MASTER1,     //单元1校准数据
	UNIT1_SLAVE_UNIT2_MASTER2,     //单元2校准数据
};

/********************** 频率设置 start ******************/
#define RF_2G_4_MIN (2400)
#define RF_2G_4_MAX (2500)

#define RF_5G_MIN	(4900)
#define RF_5G_MAX	(6000)

#define RF_3G_MIN	(3000)

#define RF_1G_MAX   (1700)
#define RF_1G_MIN	(1000)

#define RF_0G_6	    (600)
#define RF_0G_4	    (400)
#define FREQ_BASE   (400)

#define EXPAND_FREQ_SPAN (100)

#endif /* SOURCE_HAL_CAL_DATA_H_ */
