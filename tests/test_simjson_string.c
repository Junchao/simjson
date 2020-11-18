#include "unity.h"
#include "simjson.h"

void test_simjson_string_new() {
    char *str = "Hello World!";
    size_t length = strlen(str);
    SimjsonString *string = simjson_string_new(str, length);

    TEST_ASSERT_TRUE(SIMJSON_IS_STRING_TYPE(string));
    TEST_ASSERT_EQUAL_STRING(string->value, str);
    TEST_ASSERT_EQUAL_UINT64(string->length, length);

    simjson_string_free(string);
}

void test_simjson_string_new_with_empty_str() {
    char *str = "";
    size_t length = strlen(str);
    SimjsonString *string = simjson_string_new(str, length);

    TEST_ASSERT_TRUE(SIMJSON_IS_STRING_TYPE(string));
    TEST_ASSERT_EQUAL_STRING(string->value, str);
    TEST_ASSERT_EQUAL_UINT64(string->length, length);

    simjson_string_free(string);
}

void test_simjson_new_with_null_str() {
    SimjsonString *string = simjson_string_new(NULL, 0);
    TEST_ASSERT_NULL(string);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_simjson_string_new);
    RUN_TEST(test_simjson_string_new_with_empty_str);
    RUN_TEST(test_simjson_new_with_null_str);

    UNITY_END();
}
