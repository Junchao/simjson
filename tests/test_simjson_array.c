#include "unity.h"
#include "simjson.h"

static void test_string(void *json_struct, const char *expected, size_t length) {
    TEST_ASSERT_TRUE(SIMJSON_IS_STRING_TYPE(json_struct));
    SimjsonString *string = (SimjsonString *) json_struct;
    TEST_ASSERT_EQUAL_UINT64(length, string->length);
    TEST_ASSERT_EQUAL_STRING(expected, string->value);
}

void test_simjson_array_new() {
    SimjsonArray *array = simjson_array_new();
    TEST_ASSERT_TRUE(SIMJSON_IS_ARRAY_TYPE(array));
    TEST_ASSERT_EQUAL_UINT64(0, array->size);
    TEST_ASSERT_NOT_NULL(array->head);
    TEST_ASSERT_NOT_NULL(array->tail);
    simjson_array_free(array);
}

void test_simjson_array_crud() {
    SimjsonArray *array = simjson_array_new();

    size_t size = 4;
    SimjsonString *str0 = simjson_string_new("0", 1);
    SimjsonString *str1 = simjson_string_new("1", 1);
    SimjsonString *str2 = simjson_string_new("2", 1);
    SimjsonString *str3 = simjson_string_new("3", 1);

    /* * * * * * * * * * * * * * * * * */

    //1 -> 0 -> 3 -> 2
    bool insert_success;

    insert_success = simjson_array_insert(array, str0, 0);
    TEST_ASSERT_TRUE(insert_success);

    insert_success = simjson_array_insert(array, str1, 0);
    TEST_ASSERT_TRUE(insert_success);

    insert_success = simjson_array_insert(array, str2, array->size);
    TEST_ASSERT_TRUE(insert_success);

    insert_success = simjson_array_insert(array, str3, 2);
    TEST_ASSERT_TRUE(insert_success);

    TEST_ASSERT_EQUAL_UINT64(size, array->size);

    /* * * * * * * * * * * * * * * * * */

    void *index0 = simjson_array_get(array, 0);
    test_string(index0, "1", 1);

    void *index1 = simjson_array_get(array, 1);
    test_string(index1, "0", 1);

    void *index2 = simjson_array_get(array, 2);
    test_string(index2, "3", 1);

    void *index3 = simjson_array_get(array, 3);
    test_string(index3, "2", 1);

    /* * * * * * * * * * * * * * * * * */

    //1 -> 3 -> 2
    bool del_index_1 = simjson_array_delete(array, 1);
    TEST_ASSERT_TRUE(del_index_1);
    TEST_ASSERT_EQUAL_UINT64(size - 1, array->size);
    void *index1_after_del = simjson_array_get(array, 1);
    test_string(index1_after_del, "3", 1);

    //3 -> 2
    bool del_first = simjson_array_delete(array, 0);
    TEST_ASSERT_TRUE(del_first);
    TEST_ASSERT_EQUAL_UINT64(size - 2, array->size);
    void *index_first_after_del = simjson_array_get(array, 0);
    test_string(index_first_after_del, "3", 1);

    //3
    bool del_last = simjson_array_delete(array, array->size - 1);
    TEST_ASSERT_TRUE(del_last);
    void *index_last_after_del = simjson_array_get(array, array->size - 1);
    test_string(index_last_after_del, "3", 1);

    /* * * * * * * * * * * * * * * * * */

    simjson_array_free(array);
}

void test_simjson_array_crud_with_null_array() {
    void *json_struct;
    bool success;

    json_struct = simjson_array_get(NULL, 0);
    TEST_ASSERT_NULL(json_struct);

    success = simjson_array_insert(NULL, NULL, 0);
    TEST_ASSERT_FALSE(success);

    success = simjson_array_delete(NULL, 0);
    TEST_ASSERT_FALSE(success);
}

void test_simjson_array_crud_with_invalid_index() {
    SimjsonArray *array = simjson_array_new();

    SimjsonString *str0 = simjson_string_new("0", 1);
    SimjsonString *str1 = simjson_string_new("1", 1);
    simjson_array_insert(array, str0, 0);
    simjson_array_insert(array, str1, 0);

    /* * * * * * * * * * * * * * * * * */

    SimjsonString *str2 = simjson_string_new("2", 1);
    bool insert_success = simjson_array_insert(array, str2, array->size + 1);
    TEST_ASSERT_FALSE(insert_success);
    simjson_string_free(str2);

    /* * * * * * * * * * * * * * * * * */

    void *json_struct = simjson_array_get(array, array->size);
    TEST_ASSERT_NULL(json_struct);

    /* * * * * * * * * * * * * * * * * */

    bool delete_success = simjson_array_delete(array, array->size);
    TEST_ASSERT_FALSE(delete_success);

    /* * * * * * * * * * * * * * * * * */

    simjson_array_free(array);
}

void test_simjson_array_iterator() {
    SimjsonArray *array = simjson_array_new();
    SimjsonString *str0 = simjson_string_new("0", 1);
    SimjsonBoolean *boolean1 = simjson_boolean_new(true);
    simjson_array_insert(array, str0, array->size);
    simjson_array_insert(array, boolean1, array->size);

    /* * * * * * * * * * * * * * * * * */

    SimjsonArrayIterator *iterator0 = simjson_array_iterator_new(array, 0);
    TEST_ASSERT_EQUAL_UINT64(0, iterator0->cur_index);
    TEST_ASSERT_EQUAL_UINT64(array->size, iterator0->size);

    size_t index;
    while (simjson_array_iterator_has_next(iterator0)) {
        void *json_struct = simjson_array_iterator_next(iterator0, &index);
        if (index == 0) {
            test_string(json_struct, "0", 1);
        }
        else if (index == 1) {
            TEST_ASSERT_TRUE(SIMJSON_IS_BOOLEAN_TYPE(json_struct));
            SimjsonBoolean *boolean = (SimjsonBoolean *) json_struct;
            TEST_ASSERT_TRUE(boolean->value);
        }
        else {
            TEST_ASSERT_TRUE(false);
        }
    }

    /* * * * * * * * * * * * * * * * * */

    SimjsonArrayIterator *iterator1 = simjson_array_iterator_new(array, 1);
    TEST_ASSERT_EQUAL_UINT64(1, iterator1->cur_index);
    TEST_ASSERT_EQUAL_UINT64(array->size, iterator1->size);

    while (simjson_array_iterator_has_next(iterator1)) {
        void *json_struct = simjson_array_iterator_next(iterator1, &index);
        if (index == 1) {
            TEST_ASSERT_TRUE(SIMJSON_IS_BOOLEAN_TYPE(json_struct));
            SimjsonBoolean *boolean = (SimjsonBoolean *) json_struct;
            TEST_ASSERT_TRUE(boolean->value);
        }
        else {
            TEST_ASSERT_TRUE(false);
        }
    }

    /* * * * * * * * * * * * * * * * * */

    simjson_array_iterator_free(iterator0);
    simjson_array_iterator_free(iterator1);
    simjson_array_free(array);
}

void test_simjson_array_iterator_with_invalid_arg() {
    SimjsonArray *array = simjson_array_new();
    SimjsonString *str0 = simjson_string_new("0", 1);
    SimjsonBoolean *boolean1 = simjson_boolean_new(true);
    simjson_array_insert(array, str0, array->size);
    simjson_array_insert(array, boolean1, array->size);

    TEST_ASSERT_NULL(simjson_array_iterator_new(array, array->size));
    TEST_ASSERT_NULL(simjson_array_iterator_new(NULL, 0));

    simjson_array_free(array);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_simjson_array_new);
    RUN_TEST(test_simjson_array_crud);
    RUN_TEST(test_simjson_array_crud_with_null_array);
    RUN_TEST(test_simjson_array_crud_with_invalid_index);
    RUN_TEST(test_simjson_array_iterator);
    RUN_TEST(test_simjson_array_iterator_with_invalid_arg);

    UNITY_END();
}