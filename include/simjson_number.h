#ifndef SIMJSON_NUMBER_H
#define SIMJSON_NUMBER_H

#include <stdbool.h>

#include "simjson_scope.h"
#include "simjson_type.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct {
    SIMJSON_TYPE type;
    union {
        int64_t integer_value;
        double double_value;
    } value;
    bool is_integer;
} SimjsonNumber;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//创建新的number对象
SIMJSON_PUBLIC SimjsonNumber *simjson_number_new(const int64_t *integer_value, const double *double_value);

//释放number对象
SIMJSON_PUBLIC void simjson_number_free(SimjsonNumber *number);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif // SIMJSON_NUMBER_H
