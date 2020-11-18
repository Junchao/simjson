#include "unity.h"
#include "simjson.h"

void test_simjson_null_new() {
    SimjsonNull *null = simjson_null_new();
    TEST_ASSERT_TRUE(SIMJSON_IS_NULL_TYPE(null));
    TEST_ASSERT_NULL(null->value);
    simjson_null_free(null);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_simjson_null_new);
    UNITY_END();
}