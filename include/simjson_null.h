#ifndef SIMJSON_NULL_H
#define SIMJSON_NULL_H

#include "simjson_scope.h"
#include "simjson_type.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct {
    SIMJSON_TYPE type;
    void *value;
} SimjsonNull;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//创建新的null对象
SIMJSON_PUBLIC SimjsonNull *simjson_null_new();

//释放null对象
SIMJSON_PUBLIC void simjson_null_free(SimjsonNull *null);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif
