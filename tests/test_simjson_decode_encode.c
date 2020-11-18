#include <stdlib.h>

#include "unity.h"
#include "simjson.h"

static void test_json_decode_encode(const char *json_str, const char *formatted) {
    void *decoded = simjson_decode(json_str, strlen(json_str));

    size_t length;
    char *encoded = simjson_encode(decoded, &length);

    //c99
    char buf[length + 1];
    memcpy(buf, encoded, length);
    buf[length] = '\0';
    free(encoded);

    TEST_ASSERT_EQUAL_STRING(formatted, buf);

    simjson_free_json_struct(decoded);
}

static void test_string(void *json_struct, const char *expected, size_t length) {
    TEST_ASSERT_TRUE(SIMJSON_IS_STRING_TYPE(json_struct));
    SimjsonString *string = (SimjsonString *) json_struct;
    TEST_ASSERT_EQUAL_UINT64(length, string->length);
    TEST_ASSERT_EQUAL_STRING(expected, string->value);
}

void test_simjson_decode_encode_string() {
    test_json_decode_encode("\"Hello World!\"", "\"Hello World!\"");
    test_json_decode_encode("\"My favorite movie is \\\"Contact\\\"\"", "\"My favorite movie is \\\"Contact\\\"\"");
    test_json_decode_encode("    \"C Programming Language\"      ", "\"C Programming Language\"");
}

void test_simjson_decode_string_with_syntax_error() {
    char *json_str = " \"Hello  ";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = " \"World\" Hi";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "Algorithms\" ";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "Hello World!";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));
}

void test_simjson_decode_encode_number() {
    test_json_decode_encode("12345", "12345");
    test_json_decode_encode("123.45", "123.45");
    test_json_decode_encode(" 12345  ", "12345");
    test_json_decode_encode("   123.45", "123.45");
}

void test_simjson_decode_number_with_syntax_error() {
    char *json_str = "123abc";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "1.23hello45";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "abc12.3";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));
}

void test_simjson_decode_encode_boolean() {
    test_json_decode_encode("true", "true");
    test_json_decode_encode("false", "false");
    test_json_decode_encode("    true   ", "true");
    test_json_decode_encode("false    ", "false");
}

void test_simjson_decode_boolean_with_syntax_error() {
    char *json_str = "trunk";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "fall";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));
}

void test_simjson_decode_encode_null() {
    test_json_decode_encode("null", "null");
    test_json_decode_encode("  null   ", "null");
}

void test_simjson_decode_null_with_syntax_error() {
    char *json_str = "nul";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));
}

void test_simjson_decode_encode_array() {
    char *json_str = "[\"Jack\", true  ,[  123,[321 ,  [true]  ] ]  ,[],   null, [9, 7,\"China\"], false,  [   [   3.14, \"Hello\"  ,  true   ],  \"World!\" , [  ]  ]]";
    char *formatted = "[\"Jack\", true, [123, [321, [true]]], [], null, [9, 7, \"China\"], false, [[3.14, \"Hello\", true], \"World!\", []]]";
    test_json_decode_encode(json_str, formatted);

    json_str = "[   [], [  ], [[], [[[[]]]]    ], [], [  ]   ]";
    formatted = "[[], [], [[], [[[[]]]]], [], []]";
    test_json_decode_encode(json_str, formatted);
}

void test_simjson_decode_array_with_syntax_error() {
    char *json_str = "[1, 2, ]";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "[ , 1,2]";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "[1, , 2]";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "[1, 2";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "[1,2,";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "1,2]";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));
}

void test_simjson_decode_encode_object() {
    char *json_str = "{  \"name\":\"Jack\",  \"info\":[170, {\"weight\": 60} ],  \"city\"  : \"NY\"  ,\"age\"  :30 ,  \"sexy\": true, \"phone\": {\"home\": 123}  }";
    void *json_struct = simjson_decode(json_str, strlen(json_str));
    TEST_ASSERT_TRUE(SIMJSON_IS_OBJECT_TYPE(json_struct));

    SimjsonObject *object = (SimjsonObject *) json_struct;
    SimjsonObjectIterator *iterator = simjson_object_iterator_new(object);
    char *key;
    size_t key_length;

    while (simjson_object_iterator_has_next(iterator)) {
        void *iter_json_struct = simjson_object_iterator_next(iterator, &key, &key_length);
        if (strncmp(key, "name", key_length) == 0) {
            test_string(iter_json_struct, "Jack", 4);
        }
        else if (strncmp(key, "city", key_length) == 0) {
            test_string(iter_json_struct, "NY", 2);
        }
        else if (strncmp(key, "age", key_length) == 0) {
            SimjsonNumber *number = (SimjsonNumber *) iter_json_struct;
            TEST_ASSERT_EQUAL_INT64(30, number->value.integer_value);
        }
        else if (strncmp(key, "sexy", key_length) == 0) {
            SimjsonBoolean *boolean = (SimjsonBoolean *) iter_json_struct;
            TEST_ASSERT_TRUE(boolean->value);
        }
        else if (strncmp(key, "info", key_length) == 0) {
            TEST_ASSERT(SIMJSON_IS_ARRAY_TYPE(iter_json_struct));
            SimjsonArray *array = (SimjsonArray *) iter_json_struct;
            TEST_ASSERT_EQUAL_UINT64(2, array->size);
            SimjsonNumber *number = simjson_array_get(array, 0);
            TEST_ASSERT_NOT_NULL(number);
            TEST_ASSERT_EQUAL_INT64(170, number->value.integer_value);
            SimjsonObject *object_inside_array = simjson_array_get(array, 1);
            TEST_ASSERT_NOT_NULL(object_inside_array);
            number = simjson_object_get(object_inside_array, "weight", 6);
            TEST_ASSERT_NOT_NULL(number);
            TEST_ASSERT_EQUAL_INT64(60, number->value.integer_value);
        }
        else if (strncmp(key, "phone", 5) == 0) {
            TEST_ASSERT(SIMJSON_IS_OBJECT_TYPE(iter_json_struct));
            SimjsonObject *object_inside_object = (SimjsonObject *) iter_json_struct;
            SimjsonNumber *number = simjson_object_get(object_inside_object, "home", 4);
            TEST_ASSERT_NOT_NULL(number);
            TEST_ASSERT_EQUAL_INT64(123, number->value.integer_value);
        }
        else {
            TEST_ASSERT_TRUE(false);
        }
    }

    simjson_object_iterator_free(iterator);
    simjson_object_free(object);
}

void test_simjson_decode_object_with_syntax_error() {
    char *json_str = "{\"name\" :}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{:true}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{\"name}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{\"name\": true,}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{\"name\": true,, \"city\":\"BJ\"}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{\"name\", \"Jack\"}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{\"name\": \"Jack\"";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "\"name: \"Jack\"}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{true: false}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));

    json_str = "{123: \"Jack\"}";
    TEST_ASSERT_NULL(simjson_decode(json_str, strlen(json_str)));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_simjson_decode_encode_string);
    RUN_TEST(test_simjson_decode_string_with_syntax_error);

    RUN_TEST(test_simjson_decode_encode_number);
    RUN_TEST(test_simjson_decode_number_with_syntax_error);

    RUN_TEST(test_simjson_decode_encode_boolean);
    RUN_TEST(test_simjson_decode_boolean_with_syntax_error);

    RUN_TEST(test_simjson_decode_encode_null);
    RUN_TEST(test_simjson_decode_null_with_syntax_error);

    RUN_TEST(test_simjson_decode_encode_array);
    RUN_TEST(test_simjson_decode_array_with_syntax_error);

    RUN_TEST(test_simjson_decode_encode_object);
    RUN_TEST(test_simjson_decode_object_with_syntax_error);

    UNITY_END();
}
