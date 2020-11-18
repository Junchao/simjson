#include "simjson.h"

SIMJSON_PUBLIC void simjson_free_json_struct(void *json_struct) {
    if (json_struct == NULL) {
        return;
    }
    else if (SIMJSON_IS_STRING_TYPE(json_struct)) {
        simjson_string_free(json_struct);
    }
    else if (SIMJSON_IS_NUMBER_TYPE(json_struct)) {
        simjson_number_free(json_struct);
    }
    else if (SIMJSON_IS_BOOLEAN_TYPE(json_struct)) {
        simjson_boolean_free(json_struct);
    }
    else if (SIMJSON_IS_NULL_TYPE(json_struct)) {
        simjson_null_free(json_struct);
    }
    else if (SIMJSON_IS_ARRAY_TYPE(json_struct)) {
        simjson_array_free(json_struct);
    }
    else if (SIMJSON_IS_OBJECT_TYPE(json_struct)) {
        simjson_object_free(json_struct);
    }
}