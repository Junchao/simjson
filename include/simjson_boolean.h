#ifndef SIMJSON_BOOLEAN_H
#define SIMJSON_BOOLEAN_H

#include <stdbool.h>

#include "simjson_scope.h"
#include "simjson_type.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct {
    SIMJSON_TYPE type;
    bool value;
} SimjsonBoolean;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//创建新的boolean对象
SIMJSON_PUBLIC SimjsonBoolean *simjson_boolean_new(bool value);

//释放boolean对象
SIMJSON_PUBLIC void simjson_boolean_free(SimjsonBoolean *boolean);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif //SIMJSON_BOOLEAN_H
