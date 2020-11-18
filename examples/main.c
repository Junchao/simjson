#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<inttypes.h>

#include "simjson.h"

//OBJECT_BUCKET_SIZE为object内部哈希表的初始大小
#define OBJECT_BUCKET_SIZE 128

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

void encode_json();

void decode_json();

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

int main() {
    encode_json();
    decode_json();
    return 0;
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//{"married": false, "info": [170, 65.5], "city": "Shenzhen", "sex": "Male", "age": 30}
void encode_json() {
    printf("%s\n", "-----encode json example outputs-----");

    SimjsonObject *object = simjson_object_new(OBJECT_BUCKET_SIZE);

    /* * * * * * * * * * * * * * * * * */

    SimjsonString *city = simjson_string_new("Shenzhen", 8);
    simjson_object_add(object, "city", 4, city);

    SimjsonString *sex = simjson_string_new("female", 6);
    simjson_object_add(object, "sex", 3, sex);

    /* * * * * * * * * * * * * * * * * */

    int64_t i = 30;
    SimjsonNumber *age = simjson_number_new(&i, NULL);
    simjson_object_add(object, "age", 3, age);

    /* * * * * * * * * * * * * * * * * */

    SimjsonBoolean *married = simjson_boolean_new(false);
    simjson_object_add(object, "married", 7, married);

    /* * * * * * * * * * * * * * * * * */

    SimjsonArray *array = simjson_array_new();
    int64_t h = 170;
    double w = 65.5;
    SimjsonNumber *height = simjson_number_new(&h, NULL);
    SimjsonNumber *weight = simjson_number_new(NULL, &w);
    simjson_array_insert(array, height, 0);
    simjson_array_insert(array, weight, array->size);
    simjson_object_add(object, "info", 4, array);

    /* * * * * * * * * * * * * * * * * */

    char *json_str = simjson_encode(object, NULL);
    printf("%s\n\n", json_str);

    /* * * * * * * * * * * * * * * * * */

    //向object添加键值对时，键为拷贝，值为共享
    //释放object时会同时释放共享的值
    //故上面创建的对象无需手动释放
    simjson_object_free(object);
    free(json_str);
}

void decode_json() {
    printf("%s\n", "-----decode json example outputs-----");

    char *json_str = "{\"married\": false, \"info\": [170, 65.5], \"city\": \"Shenzhen\", \"sex\": \"Male\", \"age\": 30}";
    SimjsonObject *object = simjson_decode(json_str, strlen(json_str));
    assert(object != NULL);

    /* * * * * * * * * * * * * * * * * */

    SimjsonBoolean *married = simjson_object_get(object, "married", 7);
    assert(married != NULL);
    printf("%s%d\n", "married: ", married->value);

    SimjsonString *city = simjson_object_get(object, "city", 4);
    assert(city != NULL);
    printf("%s%s\n", "city: ", city->value);

    SimjsonNumber *age = simjson_object_get(object, "age", 3);
    assert(age != NULL);
    printf("%s", "age: ");
    printf("%" PRId64, age->value.integer_value);
    printf("\n");

    SimjsonArray *info = simjson_object_get(object, "info", 4);
    assert(info != NULL);
    SimjsonArrayIterator *array_iterator = simjson_array_iterator_new(info, 0);
    size_t cur_index;
    while (simjson_array_iterator_has_next(array_iterator)) {
        void *next_item = simjson_array_iterator_next(array_iterator, &cur_index);
        assert(next_item != NULL);
        // do something with next_item
    }
    simjson_array_iterator_free(array_iterator);

    /* * * * * * * * * * * * * * * * * */

    SimjsonObjectIterator *object_iterator = simjson_object_iterator_new(object);
    char *key;
    size_t key_length;
    while (simjson_object_iterator_has_next(object_iterator)) {
        void *next_item = simjson_object_iterator_next(object_iterator, &key, &key_length);
        assert(next_item != NULL);
        //do something with next_item
    }
    simjson_object_iterator_free(object_iterator);

    /* * * * * * * * * * * * * * * * * */

    simjson_object_free(object);
}

