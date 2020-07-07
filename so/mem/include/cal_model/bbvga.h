/*
 * bbvga.h
 *
 *  Created on: 2017Äê1ÔÂ5ÈÕ
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_BBVGA_H_
#define INCLUDE_CAL_MODEL_BBVGA_H_

#include "cal_model.h"
class Bbvga : public Cal_model
{
private:
	double m_cal_data[BBVGA_CODE_MAX];

public:
	Bbvga(int csv_type);
	~Bbvga(){};

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();
private:
	int get_bbvga_from_buf(char *file_buf, int len);
};

#endif /* INCLUDE_CAL_MODEL_BBVGA_H_ */
