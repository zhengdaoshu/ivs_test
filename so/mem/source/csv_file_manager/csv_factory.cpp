/*
 * csv_factory.cpp
 *
 *  Created on: 2016Äê12ÔÂ3ÈÕ
 *      Author: J00054
 */
#include <stdlib.h>
#include "../../include/csv_file_manager/csv_factory.h"
#include "../../include/csv_file_manager/csv_disk.h"
#include "../../include/csv_file_manager/csv_flash.h"
#include "../../include/base/typedef.h"
Csv_file_manager* Csv_factory::get_csv_manager(int src_type)
{
	Csv_file_manager *result = NULL;
    switch (src_type)
    {
        case src_disk:
            result = Csv_disk::getInstance();
            break;
        case src_flash:
            result = Csv_flash::getInstance();
            break;
    }
    return result;
}

