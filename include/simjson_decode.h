#ifndef SIMJSON_DECODE_H
#define SIMJSON_DECODE_H

#include "simjson_scope.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//接收json字符串，返回json对象
SIMJSON_PUBLIC void *simjson_decode(const char *json_str, size_t length);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif //SIMJSON_DECODE_H
