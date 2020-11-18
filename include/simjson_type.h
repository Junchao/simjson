#ifndef SIMJSON_TYPE_H
#define SIMJSON_TYPE_H

#include <stdbool.h>
#include <stdint.h>

#include "simjson_scope.h"

#define SIMJSON_TYPE uint8_t

#define SIMJSON_STRING_TYPE 0
#define SIMJSON_NUMBER_TYPE 1
#define SIMJSON_BOOLEAN_TYPE 2
#define SIMJSON_NULL_TYPE 3
#define SIMJSON_ARRAY_TYPE 4
#define SIMJSON_OBJECT_TYPE 5

#define SIMJSON_IS_STRING_TYPE(json_struct) !(json_struct) ? 0 : ((((unsigned) ((uint8_t *) (json_struct))[0]) == 0))
#define SIMJSON_IS_NUMBER_TYPE(json_struct) !(json_struct) ? 0 : ((((unsigned) ((uint8_t *) (json_struct))[0]) == 1))
#define SIMJSON_IS_BOOLEAN_TYPE(json_struct) !(json_struct) ? 0 : ((((unsigned) ((uint8_t *) (json_struct))[0]) == 2))
#define SIMJSON_IS_NULL_TYPE(json_struct) !(json_struct) ? 0 : ((((unsigned) ((uint8_t *) (json_struct))[0]) == 3))
#define SIMJSON_IS_ARRAY_TYPE(json_struct) !(json_struct) ? 0 : ((((unsigned) ((uint8_t *) (json_struct))[0]) == 4))
#define SIMJSON_IS_OBJECT_TYPE(json_struct) !(json_struct) ? 0 : ((((unsigned) ((uint8_t *) (json_struct))[0]) == 5))

SIMJSON_PUBLIC void simjson_free_json_struct(void *json_struct);

#endif // SIMJSON_TYPE_H
