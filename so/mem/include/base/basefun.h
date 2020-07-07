/*
 * basefun.h
 *
 *  Created on: 2016Äê11ÔÂ25ÈÕ
 *      Author: J00054
 */

#ifndef INCLUDE_BASE_BASEFUN_H_
#define INCLUDE_BASE_BASEFUN_H_
#include "../base/cal_error.h"

#define err_break(err) if (err != ERR_RET_OK)	  \
					   {						  \
							break;				  \
					   }


#define err_continue(err) if (err != ERR_RET_OK)  \
					      {						  \
						      continue;			  \
					      }


#endif /* INCLUDE_BASE_BASEFUN_H_ */
