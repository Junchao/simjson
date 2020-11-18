#ifndef SIMJSON_ARRAY_H
#define SIMJSON_ARRAY_H

#include <stdbool.h>

#include "simjson_scope.h"
#include "simjson_type.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

typedef struct SimjsonArrayItem SimjsonArrayItem;

typedef struct {
    SIMJSON_TYPE type;
    SimjsonArrayItem *head;
    SimjsonArrayItem *tail;
    size_t size;
} SimjsonArray;

typedef struct {
    SimjsonArrayItem *cur_item;
    size_t cur_index;
    size_t size;
} SimjsonArrayIterator;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//创建新的array对象
SIMJSON_PUBLIC SimjsonArray *simjson_array_new();

//释放array对象
SIMJSON_PUBLIC void simjson_array_free(SimjsonArray *array);

//在index处插入新元素
//若index == array.size，即表尾添加元素
SIMJSON_PUBLIC bool simjson_array_insert(SimjsonArray *array, void *json_struct, size_t index);

//获取index处的元素
SIMJSON_PUBLIC void *simjson_array_get(SimjsonArray *array, size_t index);

//删除index处的元素
SIMJSON_PUBLIC bool simjson_array_delete(SimjsonArray *array, size_t index);

//创建迭代器
SIMJSON_PUBLIC SimjsonArrayIterator *simjson_array_iterator_new(SimjsonArray *array, size_t start_index);

//释放迭代器
SIMJSON_PUBLIC void simjson_array_iterator_free(SimjsonArrayIterator *iterator);

//迭代器是否有下一个元素
SIMJSON_PUBLIC bool simjson_array_iterator_has_next(SimjsonArrayIterator *iterator);

//获取迭代器的下一个元素
//若index不为NULL，写入当前元素索引
SIMJSON_PUBLIC void *simjson_array_iterator_next(SimjsonArrayIterator *iterator, size_t *index);

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#endif // SIMJSON_ARRAY_H
