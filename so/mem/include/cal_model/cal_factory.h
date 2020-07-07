/*
 * cal_factory.h
 *
 *  Created on: 2016Äê12ÔÂ7ÈÕ
 *      Author: J00054
 */

#ifndef INCLUDE_CAL_MODEL_CAL_FACTORY_H_
#define INCLUDE_CAL_MODEL_CAL_FACTORY_H_
#include "cal_model.h"
class Cal_factory
{
public:
	static Cal_model* create_cal_model(int cal_type, int csv_type);
};

#endif /* INCLUDE_CAL_MODEL_CAL_FACTORY_H_ */
