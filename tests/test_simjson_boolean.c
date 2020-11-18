#include "unity.h"
#include "simjson.h"

void test_simjson_boolean_new() {
    SimjsonBoolean *boolean_true = simjson_boolean_new(true);
    TEST_ASSERT_TRUE(SIMJSON_IS_BOOLEAN_TYPE(boolean_true));
    TEST_ASSERT_TRUE(boolean_true->value);

    SimjsonBoolean *boolean_false = simjson_boolean_new(false);
    TEST_ASSERT_TRUE(SIMJSON_IS_BOOLEAN_TYPE(boolean_false));
    TEST_ASSERT_FALSE(boolean_false->value);

    simjson_boolean_free(boolean_true);
    simjson_boolean_free(boolean_false);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_simjson_boolean_new);
    UNITY_END();
}

