#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "simjson_scope.h"
#include "simjson_string.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PUBLIC SimjsonString *simjson_string_new(const char *value, size_t length) {
    if (value == NULL) {
        DEBUG_INFO("value is NULL");
        return NULL;
    }

    SimjsonString *string = malloc(sizeof(SimjsonString));
    if (string == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    string->value = malloc(sizeof(length + 1));
    if (string->value == NULL) {
        DEBUG_INFO(strerror(errno));
        free(string);
        return NULL;
    }

    memcpy(string->value, value, length);
    string->value[length] = '\0';
    string->length = length;
    string->type = SIMJSON_STRING_TYPE;

    return string;
}

SIMJSON_PUBLIC void simjson_string_free(SimjsonString *string) {
    if (string != NULL) {
        free(string->value);
        free(string);
    }
}