#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "simjson.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

const static uint8_t INVALID_CHAR = 32;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct {
    const char *json_str;
    size_t length;
    size_t offset;
} JsonBuf;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PRIVATE JsonBuf *json_buf_new(const char *json_str, size_t length) {
    JsonBuf *json_buf = malloc(sizeof(JsonBuf));
    if (json_buf == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    json_buf->json_str = json_str;
    json_buf->length = length;
    json_buf->offset = 0;

    return json_buf;
}

SIMJSON_PRIVATE void json_buf_free(JsonBuf *json_buf) {
    free(json_buf);
}

SIMJSON_PRIVATE inline char json_buf_cur_char(JsonBuf *json_buf) {
    return (json_buf->json_str + json_buf->offset)[0];
}

SIMJSON_PRIVATE inline const char *json_buf_cur_str(JsonBuf *json_buf) {
    return json_buf->json_str + json_buf->offset;
}

SIMJSON_PRIVATE inline bool json_buf_reach_end(JsonBuf *json_buf) {
    return json_buf->offset >= json_buf->length;
}

SIMJSON_PRIVATE inline bool is_string(char c) {
    return c == '\"';
}

SIMJSON_PRIVATE inline bool is_number(char c) {
    return c == '-' || (c >= '0' && c <= '9');
}

SIMJSON_PRIVATE inline bool is_boolean(char c) {
    return c == 't' || c == 'f';
}

SIMJSON_PRIVATE inline bool is_null(char c) {
    return c == 'n';
}

SIMJSON_PRIVATE inline bool is_array(char c) {
    return c == '[';
}

SIMJSON_PRIVATE inline bool is_object(char c) {
    return c == '{';
}

SIMJSON_PRIVATE inline void skip_ws(JsonBuf *json_buf) {
    while (!json_buf_reach_end(json_buf) && json_buf_cur_char(json_buf) <= INVALID_CHAR) {
        json_buf->offset++;
    }
}

SIMJSON_PRIVATE inline bool reach_array_end(JsonBuf *json_buf) {
    return json_buf_cur_char(json_buf) == ']';
}

SIMJSON_PRIVATE inline bool reach_object_end(JsonBuf *json_buf) {
    return json_buf_cur_char(json_buf) == '}';
}

SIMJSON_PRIVATE bool get_string_length(JsonBuf *json_buf, size_t *length) {
    size_t len = 0;

    while (!json_buf_reach_end(json_buf)) {
        if (json_buf_cur_char(json_buf) == '\\') {
            json_buf->offset++;
            char c = json_buf_cur_char(json_buf);
            //todo 处理unicode相关(\u)
            if (c == '\"' || c == '\\' || c == '/' || c == 'b' || c == 'n' || c == 'r' || c == 't') {
                json_buf->offset++;
                len += 2;
            }
            else {
                return false;
            }
        }
        else if (json_buf_cur_char(json_buf) == '\"') {
            assert(length != NULL);
            json_buf->offset -= len;
            *length = len;
            return true;
        }
        else {
            json_buf->offset++;
            len++;
        }
    }

    return false;
}

SIMJSON_PRIVATE bool decode_object_key(JsonBuf *json_buf, const char **key_start, size_t *key_length) {
    skip_ws(json_buf);

    if (!is_string(json_buf_cur_char(json_buf))) {
        return false;
    }

    json_buf->offset++;
    if (!get_string_length(json_buf, key_length)) {
        return false;
    }

    *key_start = json_buf_cur_str(json_buf);
    json_buf->offset += *key_length + 1;
    return true;
}

SIMJSON_PRIVATE bool get_integer(JsonBuf *json_buf, int64_t *value, size_t *length) {
    errno = 0;
    char *int_end;
    int64_t integer_value = strtol(json_buf_cur_str(json_buf), &int_end, 10);

    if (json_buf_cur_str(json_buf) == int_end) {
        return false;
    }
    else if (errno != 0) {
        DEBUG_INFO(strerror(errno));
        return false;
    }
    else if (*int_end == 'e' || *int_end == 'E' || *int_end == '.') {
        return false;
    }

    assert(value != NULL && length != NULL);
    *value = integer_value;
    *length = int_end - json_buf_cur_str(json_buf);

    return true;
}

SIMJSON_PRIVATE bool get_double(JsonBuf *json_buf, double *value, size_t *length) {
    errno = 0;
    char *double_end;
    double double_value = strtod(json_buf_cur_str(json_buf), &double_end);

    if (json_buf_cur_str(json_buf) == double_end) {
        return false;
    }
    else if (errno != 0) {
        DEBUG_INFO(strerror(errno));
        return false;
    }

    assert(value != NULL && length != NULL);
    *value = double_value;
    *length = double_end - json_buf_cur_str(json_buf);

    return true;
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PRIVATE void *decode(JsonBuf *json_buf);

SIMJSON_PRIVATE void *decode_string(JsonBuf *json_buf) {
    json_buf->offset++;

    size_t length;
    if (!get_string_length(json_buf, &length)) {
        DEBUG_INFO("string type syntax error");
        return NULL;
    }

    SimjsonString *string = simjson_string_new(json_buf_cur_str(json_buf), length);
    if (string == NULL) {
        return NULL;
    }
    json_buf->offset = json_buf->offset + length + 1;

    return string;
}

SIMJSON_PRIVATE void *decode_number(JsonBuf *json_buf) {
    size_t length;
    int64_t integer_value;
    double double_value;

    if (get_integer(json_buf, &integer_value, &length)) {
        SimjsonNumber *number = simjson_number_new(&integer_value, NULL);
        if (number == NULL) {
            return NULL;
        }
        json_buf->offset += length;
        return number;
    }
    else if (get_double(json_buf, &double_value, &length)) {
        SimjsonNumber *number = simjson_number_new(NULL, &double_value);
        if (number == NULL) {
            return NULL;
        }
        json_buf->offset += length;
        return number;
    }
    else {
        DEBUG_INFO("number type syntax error");
        return NULL;
    }
}

SIMJSON_PRIVATE void *decode_boolean(JsonBuf *json_buf) {
    if (strncmp(json_buf_cur_str(json_buf), "true", 4) == 0) {
        SimjsonBoolean *boolean = simjson_boolean_new(true);
        if (boolean == NULL) {
            return NULL;
        }
        json_buf->offset += 4;
        return boolean;
    }
    else if (strncmp(json_buf_cur_str(json_buf), "false", 5) == 0) {
        SimjsonBoolean *boolean = simjson_boolean_new(false);
        if (boolean == NULL) {
            return NULL;
        }
        json_buf->offset += 5;
        return boolean;
    }
    else {
        DEBUG_INFO("boolean type syntax error");
        return NULL;
    }
}

SIMJSON_PRIVATE void *decode_null(JsonBuf *json_buf) {
    if (strncmp(json_buf_cur_str(json_buf), "null", 4) == 0) {
        SimjsonNull *null = simjson_null_new();
        if (null == NULL) {
            return NULL;
        }
        json_buf->offset += 4;
        return null;
    }
    else {
        DEBUG_INFO("null type syntax error");
        return NULL;
    }
}

SIMJSON_PRIVATE void *decode_array(JsonBuf *json_buf) {
    SimjsonArray *array = simjson_array_new();
    if (array == NULL) {
        return NULL;
    }

    json_buf->offset++;
    skip_ws(json_buf);
    if (reach_array_end(json_buf)) {
        goto SUCCESS;
    }

    while (true) {
        void *json_struct = decode(json_buf);
        if (json_struct == NULL) {
            goto FAILED;
        }
        simjson_array_insert(array, json_struct, array->size);

        skip_ws(json_buf);

        if (reach_array_end(json_buf)) {
            goto SUCCESS;
        }
        else if (json_buf_cur_char(json_buf) == ',') {
            json_buf->offset++;
        }
        else {
            goto FAILED;
        }
    }

    FAILED:
    DEBUG_INFO("array type syntax error");
    simjson_array_free(array);
    return NULL;

    SUCCESS:
    json_buf->offset++;
    return array;
}

SIMJSON_PRIVATE void *decode_object(JsonBuf *json_buf) {
    SimjsonObject *object = simjson_object_new(0);
    if (object == NULL) {
        return NULL;
    }

    json_buf->offset++;
    skip_ws(json_buf);
    if (reach_object_end(json_buf)) {
        goto SUCCESS;
    }

    while (true) {
        const char *key_start;
        size_t key_length;
        if (!decode_object_key(json_buf, &key_start, &key_length)) {
            goto FAILED;
        }

        skip_ws(json_buf);

        if (json_buf_cur_char(json_buf) != ':') {
            goto FAILED;
        }
        json_buf->offset++;

        void *value = decode(json_buf);
        if (value == NULL) {
            goto FAILED;
        }
        simjson_object_add(object, key_start, key_length, value);

        skip_ws(json_buf);

        if (reach_object_end(json_buf)) {
            goto SUCCESS;
        }
        else if (json_buf_cur_char(json_buf) == ',') {
            json_buf->offset++;
        }
        else {
            goto FAILED;
        }
    }

    FAILED:
    DEBUG_INFO("object type syntax error");
    simjson_object_free(object);
    return NULL;

    SUCCESS:
    json_buf->offset++;
    return object;
}

SIMJSON_PRIVATE void *decode(JsonBuf *json_buf) {
    skip_ws(json_buf);
    if (json_buf_reach_end(json_buf)) {
        return NULL;
    }

    if (is_string(json_buf_cur_char(json_buf))) {
        return decode_string(json_buf);
    }
    else if (is_number(json_buf_cur_char(json_buf))) {
        return decode_number(json_buf);
    }
    else if (is_boolean(json_buf_cur_char(json_buf))) {
        return decode_boolean(json_buf);
    }
    else if (is_null(json_buf_cur_char(json_buf))) {
        return decode_null(json_buf);
    }
    else if (is_array(json_buf_cur_char(json_buf))) {
        return decode_array(json_buf);
    }
    else if (is_object(json_buf_cur_char(json_buf))) {
        return decode_object(json_buf);
    }
    else {
        DEBUG_INFO("Unknown json type");
        return NULL;
    }
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PUBLIC void *simjson_decode(const char *json_str, size_t length) {
    if (json_str == NULL) {
        DEBUG_INFO("json_str is NULL");
        return NULL;
    }

    JsonBuf *json_buf = json_buf_new(json_str, length);
    if (json_buf == NULL) {
        return NULL;
    }

    void *json_struct = decode(json_buf);

    skip_ws(json_buf);
    if (json_struct == NULL || !json_buf_reach_end(json_buf)) {
        json_buf_free(json_buf);
        simjson_free_json_struct(json_struct);
        return NULL;
    }
    else {
        json_buf_free(json_buf);
        return json_struct;
    }
}
