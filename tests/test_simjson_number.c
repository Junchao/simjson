#include "unity.h"
#include "simjson.h"

void test_simjson_number_new() {
    long i = 12345;
    SimjsonNumber *number_integer = simjson_number_new(&i, NULL);
    TEST_ASSERT_TRUE(SIMJSON_IS_NUMBER_TYPE(number_integer));
    TEST_ASSERT_EQUAL_INT64(i, number_integer->value.integer_value);

    double d = 123.45;
    SimjsonNumber *number_double = simjson_number_new(NULL, &d);
    TEST_ASSERT_TRUE(SIMJSON_IS_NUMBER_TYPE(number_double));
    TEST_ASSERT_TRUE(d == number_double->value.double_value);

    simjson_number_free(number_integer);
    simjson_number_free(number_double);
}

void test_simjson_number_with_null() {
    SimjsonNumber *number = simjson_number_new(NULL, NULL);
    TEST_ASSERT_NULL(number);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_simjson_number_new);
    RUN_TEST(test_simjson_number_with_null);

    UNITY_END();
}
