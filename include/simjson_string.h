#ifndef SIMJSON_STRING_H
#define SIMJSON_STRING_H

#include <string.h>

#include "simjson_type.h"
#include "simjson_scope.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct {
    SIMJSON_TYPE type;
    size_t length;
    char *value;
} SimjsonString;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//创建新的string对象
SIMJSON_PUBLIC SimjsonString *simjson_string_new(const char *value, size_t length);

//释放string对象
SIMJSON_PUBLIC void simjson_string_free(SimjsonString *string);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif //SIMJSON_STRING_H
