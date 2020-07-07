/*
 * ocxo.h
 *
 *  Created on: 2016Äê11ÔÂ30ÈÕ
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_OCXO_H_
#define INCLUDE_CAL_MODEL_OCXO_H_
#include "cal_model.h"

class Ocxo :public Cal_model
{
private:
	int m_ocxo_code;
public:
	Ocxo(int csv_type);
	~Ocxo(){};

	virtual int wt_calibration_initial(char *buf);

	virtual double get_calibration_data(char *ref_buf);
	virtual void calibration_data_reset();
private:
	int get_ocxo_code_from_buf(char *file_buf, int len);
};

#endif /* INCLUDE_CAL_MODEL_OCXO_H_ */
