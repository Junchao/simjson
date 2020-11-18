#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

#include "simjson_number.h"
#include "simjson_type.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SimjsonNumber *simjson_number_new(const int64_t *integer_value, const double *double_value) {
    if (integer_value == NULL && double_value == NULL) {
        DEBUG_INFO("integer_value, double_value can't both be NULL");
        return NULL;
    }

    if (integer_value != NULL && double_value != NULL) {
        DEBUG_INFO("one of integer_value/double_value must be NULL");
        return NULL;
    }

    SimjsonNumber *number = malloc(sizeof(SimjsonNumber));
    if (number == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    if (integer_value != NULL) {
        number->value.integer_value = *integer_value;
        number->is_integer = true;
    }
    else {
        number->value.double_value = *double_value;
        number->is_integer = false;
    }

    number->type = SIMJSON_NUMBER_TYPE;
    return number;
}

SIMJSON_PUBLIC void simjson_number_free(SimjsonNumber *number) {
    free(number);
}
