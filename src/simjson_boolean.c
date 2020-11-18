#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

#include "simjson_boolean.h"
#include "log.h"

SimjsonBoolean *simjson_boolean_new(bool value) {
    SimjsonBoolean *boolean = malloc(sizeof(SimjsonBoolean));
    if (boolean == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }
    boolean->value = value;
    boolean->type = SIMJSON_BOOLEAN_TYPE;
    return boolean;
}

SIMJSON_PUBLIC void simjson_boolean_free(SimjsonBoolean *boolean) {
    free(boolean);
}
