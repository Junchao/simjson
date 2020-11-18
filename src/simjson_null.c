#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

#include "simjson_null.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SimjsonNull *simjson_null_new() {
    SimjsonNull *null = malloc(sizeof(SimjsonNull));
    if (null == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }
    null->value = NULL;
    null->type = SIMJSON_NULL_TYPE;
    return null;
}

SIMJSON_PUBLIC void simjson_null_free(SimjsonNull *null) {
    free(null);
}


