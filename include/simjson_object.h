#ifndef SIMJSON_OBJECT_H
#define SIMJSON_OBJECT_H

#include <stdbool.h>

#include "simjson_scope.h"
#include "simjson_type.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct SimjsonObjectItem SimjsonObjectItem;

typedef struct {
    SIMJSON_TYPE type;
    SimjsonObjectItem **buckets;
    size_t bucket_size;
    size_t item_size;
} SimjsonObject;

typedef struct {
    SimjsonObject *object;
    SimjsonObjectItem *cur_item;
    size_t cur_item_index;
    size_t cur_bucket_index;
} SimjsonObjectIterator;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//创建新的object对象
SIMJSON_PUBLIC SimjsonObject *simjson_object_new(size_t bucket_size);

//释放object对象
SIMJSON_PUBLIC void simjson_object_free(SimjsonObject *object);

//添加键值对
SIMJSON_PUBLIC bool simjson_object_add(SimjsonObject *object, const char *key, size_t key_length, void *json_struct);

//获取键对应的值
SIMJSON_PUBLIC void *simjson_object_get(SimjsonObject *object, const char *key, size_t key_length);

//删除键值对
SIMJSON_PUBLIC bool simjson_object_delete(SimjsonObject *object, const char *key, size_t key_length);

//创建迭代器
SIMJSON_PUBLIC SimjsonObjectIterator *simjson_object_iterator_new(SimjsonObject *object);

//释放迭代器
SIMJSON_PUBLIC void simjson_object_iterator_free(SimjsonObjectIterator *iterator);

//迭代器是否还有下一个元素
SIMJSON_PUBLIC bool simjson_object_iterator_has_next(SimjsonObjectIterator *iterator);

//获取迭代器的下一个元素
SIMJSON_PUBLIC void *simjson_object_iterator_next(SimjsonObjectIterator *iterator, char **key, size_t *key_length);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif // SIMJSON_OBJECT_H
