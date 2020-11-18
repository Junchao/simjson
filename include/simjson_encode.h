#ifndef SIMJSON_ENCODE_H
#define SIMJSON_ENCODE_H

#include "simjson_scope.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//接收json对象，返回json字符串
//调用者负责free返回的字符串
SIMJSON_PUBLIC char *simjson_encode(void *json_struct, size_t *json_str_length);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif //SIMJSON_ENCODE_H
