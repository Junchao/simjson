#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "simjson.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

const static size_t BUF_INITIAL_SIZE = 32;
const static uint8_t GROW_FACTOR = 2;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct {
    char *buf;
    size_t size;
    size_t length;
} JsonBuf;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PRIVATE JsonBuf *json_buf_new() {
    JsonBuf *json_buf = malloc(sizeof(JsonBuf));
    if (json_buf == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    json_buf->buf = malloc(BUF_INITIAL_SIZE);
    if (json_buf->buf == NULL) {
        DEBUG_INFO(strerror(errno));
        free(json_buf);
        return NULL;
    }

    json_buf->size = BUF_INITIAL_SIZE;
    json_buf->length = 0;

    return json_buf;
}

SIMJSON_PRIVATE void json_buf_free(JsonBuf *json_buf) {
    free(json_buf->buf);
    free(json_buf);
}

SIMJSON_PRIVATE bool json_buf_grow(JsonBuf *json_buf, size_t needed) {
    char *new_buf = realloc(json_buf->buf, (json_buf->size + needed) * GROW_FACTOR);
    if (new_buf == NULL) {
        DEBUG_INFO(strerror(errno));
        return false;
    }
    else {
        json_buf->buf = new_buf;
        json_buf->size = (json_buf->size + needed) * 2;
        return true;
    }
}

SIMJSON_PRIVATE inline bool json_buf_has_space_for(JsonBuf *json_buf, size_t needed) {
    return json_buf->length + needed <= json_buf->size;
}

SIMJSON_PRIVATE bool json_buf_append(JsonBuf *json_buf, const char *str, size_t length) {
    if (!json_buf_has_space_for(json_buf, length) && !json_buf_grow(json_buf, length)) {
        return false;
    }
    memcpy(json_buf->buf + json_buf->length, str, length);
    json_buf->length += length;
    return true;
}

SIMJSON_PRIVATE char *json_buf_to_string(JsonBuf *json_buf) {
    char *buf = malloc(json_buf->length + 1);
    if (buf == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }
    memcpy(buf, json_buf->buf, json_buf->length);
    buf[json_buf->length] = '\0';
    return buf;
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PRIVATE bool encode(JsonBuf *json_buf, void *json_struct);

SIMJSON_PRIVATE bool encode_string(JsonBuf *json_buf, void *json_struct) {
    SimjsonString *string = (SimjsonString *) json_struct;
    if (!json_buf_append(json_buf, "\"", 1) ||
        !json_buf_append(json_buf, string->value, string->length) ||
        !json_buf_append(json_buf, "\"", 1)) {
        return false;
    }
    return true;
}

SIMJSON_PRIVATE bool encode_number(JsonBuf *json_buf, void *json_struct) {
    SimjsonNumber *number = (SimjsonNumber *) json_struct;
    size_t length;

    if (number->is_integer) {
        length = snprintf(NULL, 0, "%ld", number->value.integer_value);
    }
    else {
        //todo 精度
        length = snprintf(NULL, 0, "%g", number->value.double_value);
    }

    char *buf = malloc(length + 1);
    if (buf == NULL) {
        DEBUG_INFO(strerror(errno));
        return false;
    }

    if (number->is_integer) {
        snprintf(buf, length + 1, "%ld", number->value.integer_value);
    }
    else {
        //todo 精度
        snprintf(buf, length + 1, "%g", number->value.double_value);
    }

    return json_buf_append(json_buf, buf, length);
}

SIMJSON_PRIVATE bool encode_boolean(JsonBuf *json_buf, void *json_struct) {
    SimjsonBoolean *boolean = (SimjsonBoolean *) json_struct;
    if (boolean->value == true) {
        return json_buf_append(json_buf, "true", 4);
    }
    else {
        return json_buf_append(json_buf, "false", 5);
    }
}

SIMJSON_PRIVATE bool encode_null(JsonBuf *json_buf, void *json_struct) {
    return json_buf_append(json_buf, "null", 4);
}

SIMJSON_PRIVATE bool encode_array(JsonBuf *json_buf, void *json_struct) {
    if (!json_buf_append(json_buf, "[", 1)) {
        return false;
    }

    SimjsonArray *array = (SimjsonArray *) json_struct;
    if (array->size == 0) {
        return json_buf_append(json_buf, "]", 1);
    }

    SimjsonArrayIterator *iterator = simjson_array_iterator_new(array, 0);
    if (iterator == NULL) {
        return false;
    }

    size_t index;
    while (simjson_array_iterator_has_next(iterator)) {
        void *iter_json_struct = simjson_array_iterator_next(iterator, &index);
        if (!encode(json_buf, iter_json_struct)) {
            goto FAILED;
        }
        if (index < array->size - 1) {
            if (!json_buf_append(json_buf, ", ", 2)) {
                goto FAILED;
            }
        }
    }

    if (!json_buf_append(json_buf, "]", 1)) {
        goto FAILED;
    };

    return true;

    FAILED:
    simjson_array_iterator_free(iterator);
    return false;
}

SIMJSON_PRIVATE bool encode_object(JsonBuf *json_buf, void *json_struct) {
    if (!json_buf_append(json_buf, "{", 1)) {
        return false;
    }

    SimjsonObject *object = (SimjsonObject *) json_struct;
    SimjsonObjectIterator *iterator = simjson_object_iterator_new(object);
    if (iterator == NULL) {
        return false;
    }

    char *key;
    size_t key_length;
    size_t item_size = object->item_size;

    while (simjson_object_iterator_has_next(iterator)) {
        void *iter_json_struct = simjson_object_iterator_next(iterator, &key, &key_length);
        if (!json_buf_append(json_buf, "\"", 1) ||
            !json_buf_append(json_buf, key, key_length) ||
            !json_buf_append(json_buf, "\"", 1) ||
            !json_buf_append(json_buf, ": ", 2)) {
            goto FAILED;
        }
        if (!encode(json_buf, iter_json_struct)) {
            goto FAILED;
        }
        if (--item_size) {
            if (!json_buf_append(json_buf, ", ", 2)) {
                goto FAILED;
            }
        }
    }

    if (!json_buf_append(json_buf, "}", 1)) {
        goto FAILED;
    }

    return true;

    FAILED:
    simjson_object_iterator_free(iterator);
    return false;
}

SIMJSON_PRIVATE bool encode(JsonBuf *json_buf, void *json_struct) {
    if (SIMJSON_IS_STRING_TYPE(json_struct)) {
        return encode_string(json_buf, json_struct);
    }
    else if (SIMJSON_IS_NUMBER_TYPE(json_struct)) {
        return encode_number(json_buf, json_struct);
    }
    else if (SIMJSON_IS_BOOLEAN_TYPE(json_struct)) {
        return encode_boolean(json_buf, json_struct);
    }
    else if (SIMJSON_IS_NULL_TYPE(json_struct)) {
        return encode_null(json_buf, json_struct);
    }
    else if (SIMJSON_IS_ARRAY_TYPE(json_struct)) {
        return encode_array(json_buf, json_struct);
    }
    else if (SIMJSON_IS_OBJECT_TYPE(json_struct)) {
        return encode_object(json_buf, json_struct);
    }
    else {
        DEBUG_INFO("Unknown data type");
        return false;
    }
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PUBLIC char *simjson_encode(void *json_struct, size_t *json_str_length) {
    if (json_struct == NULL) {
        DEBUG_INFO("json_struct is NULL");
        return NULL;
    }

    JsonBuf *json_buf = json_buf_new();
    if (json_buf == NULL) {
        return NULL;
    }

    bool success = encode(json_buf, json_struct);
    if (success) {
        char *json_str = json_buf_to_string(json_buf);
        if (json_str != NULL) {
            if (json_str_length != NULL) {
                *json_str_length = json_buf->length;
            }
        }
        json_buf_free(json_buf);
        return json_str;
    }

    DEBUG_INFO("encode failed");
    json_buf_free(json_buf);
    return NULL;
}
