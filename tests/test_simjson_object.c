#include "unity.h"
#include "simjson.h"

static void test_string(void *json_struct, const char *expected, size_t length) {
    TEST_ASSERT_NOT_NULL(json_struct);
    TEST_ASSERT_TRUE(SIMJSON_IS_STRING_TYPE(json_struct));
    SimjsonString *string = (SimjsonString *) json_struct;
    TEST_ASSERT_EQUAL_UINT64(length, string->length);
    TEST_ASSERT_EQUAL_STRING(expected, string->value);
}

void test_simjson_object_new() {
    SimjsonObject *object = simjson_object_new(0);
    TEST_ASSERT_TRUE(SIMJSON_IS_OBJECT_TYPE(object));
    TEST_ASSERT_EQUAL_UINT64(0, object->item_size);
    simjson_object_free(object);
}

void test_simjson_object_crud() {
    SimjsonObject *object = simjson_object_new(0);

    size_t size = 3;
    SimjsonString *str_name = simjson_string_new("Jack", 4);
    SimjsonString *str_city = simjson_string_new("Shanghai", 8);
    SimjsonString *str_company = simjson_string_new("Amazon", 6);

    /* * * * * * * * * * * * * * * * * */

    bool add_success;

    add_success = simjson_object_add(object, "name", 4, str_name);
    TEST_ASSERT_TRUE(add_success);

    add_success = simjson_object_add(object, "city", 4, str_city);
    TEST_ASSERT_TRUE(add_success);

    add_success = simjson_object_add(object, "company", 7, str_company);
    TEST_ASSERT_TRUE(add_success);

    TEST_ASSERT_EQUAL_UINT64(size, object->item_size);

    /* * * * * * * * * * * * * * * * * */

    void *name = simjson_object_get(object, "name", 4);
    test_string(name, "Jack", 4);

    void *city = simjson_object_get(object, "city", 4);
    test_string(city, "Shanghai", 8);

    void *company = simjson_object_get(object, "company", 7);
    test_string(company, "Amazon", 6);

    /* * * * * * * * * * * * * * * * * */

    bool delete_success;
    delete_success = simjson_object_delete(object, "city", 4);
    TEST_ASSERT_TRUE(delete_success);
    TEST_ASSERT_NULL(simjson_object_get(object, "city", 4));

    //测试删除之后能否正常获取其他键值对
    company = simjson_object_get(object, "company", 7);
    test_string(company, "Amazon", 6);
    name = simjson_object_get(object, "name", 4);
    test_string(name, "Jack", 4);

    /* * * * * * * * * * * * * * * * * */

    simjson_object_free(object);
}

void test_simjson_object_add_with_exists_key() {
    SimjsonObject *object = simjson_object_new(0);
    SimjsonString *str_name = simjson_string_new("Jack", 4);
    simjson_object_add(object, "name", 4, str_name);
    TEST_ASSERT_FALSE(simjson_object_add(object, "name", 4, str_name));
    simjson_object_free(object);
}

void test_simjson_get_delete_not_exists() {
    SimjsonObject *object = simjson_object_new(0);
    TEST_ASSERT_NULL(simjson_object_get(object, "key", 3));
    TEST_ASSERT_FALSE(simjson_object_delete(object, "key", 3));
    simjson_object_free(object);
}

void test_simjson_object_crud_with_invalid_arg() {
    SimjsonObject *object = simjson_object_new(0);
    SimjsonString *str_name = simjson_string_new("Jack", 4);

    TEST_ASSERT_FALSE(simjson_object_add(NULL, "name", 4, str_name));
    TEST_ASSERT_FALSE(simjson_object_add(object, NULL, 4, str_name));
    TEST_ASSERT_FALSE(simjson_object_add(object, "", 0, str_name));
    TEST_ASSERT_FALSE(simjson_object_add(object, "name", 4, NULL));

    simjson_string_free(str_name);

    /* * * * * * * * * * * * * * * * * */

    TEST_ASSERT_NULL(simjson_object_get(NULL, "name", 4));
    TEST_ASSERT_NULL(simjson_object_get(object, NULL, 4));
    TEST_ASSERT_NULL(simjson_object_get(object, "", 0));

    /* * * * * * * * * * * * * * * * * */

    TEST_ASSERT_FALSE(simjson_object_delete(NULL, "name", 4));
    TEST_ASSERT_FALSE(simjson_object_delete(object, NULL, 4));
    TEST_ASSERT_FALSE(simjson_object_delete(object, "", 0));

    /* * * * * * * * * * * * * * * * * */

    simjson_object_free(object);
}

void test_simjson_object_iterator() {
    SimjsonObject *object = simjson_object_new(0);
    SimjsonString *str_name = simjson_string_new("Amazon", 6);
    SimjsonBoolean *boolean = simjson_boolean_new(true);
    simjson_object_add(object, "name", 4, str_name);
    simjson_object_add(object, "on public", strlen("on public"), boolean);

    /* * * * * * * * * * * * * * * * * */

    SimjsonObjectIterator *iterator = simjson_object_iterator_new(object);
    TEST_ASSERT_EQUAL_UINT64(0, iterator->cur_bucket_index);
    TEST_ASSERT_EQUAL_UINT64(0, iterator->cur_item_index);

    char *key;
    size_t key_length;
    while (simjson_object_iterator_has_next(iterator)) {
        void *json_struct = simjson_object_iterator_next(iterator, &key, &key_length);
        if (strncmp(key, "name", key_length) == 0) {
            test_string(json_struct, "Amazon", 6);
        }
        else if (strncmp(key, "on public", key_length) == 0) {
            TEST_ASSERT_TRUE(SIMJSON_IS_BOOLEAN_TYPE(json_struct));
            SimjsonBoolean *b = (SimjsonBoolean *) json_struct;
            TEST_ASSERT_TRUE(b->value);
        }
        else {
            TEST_ASSERT_TRUE(false);
        }
    }

    /* * * * * * * * * * * * * * * * * */

    simjson_object_iterator_free(iterator);
    simjson_object_free(object);
}

void test_simjson_object_iterator_with_invalid_arg() {
    TEST_ASSERT_NULL(simjson_object_iterator_new(NULL));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_simjson_object_new);
    RUN_TEST(test_simjson_object_crud);
    RUN_TEST(test_simjson_object_add_with_exists_key);
    RUN_TEST(test_simjson_get_delete_not_exists);
    RUN_TEST(test_simjson_object_crud_with_invalid_arg);
    RUN_TEST(test_simjson_object_iterator);
    RUN_TEST(test_simjson_object_iterator_with_invalid_arg);

    UNITY_END();
}

