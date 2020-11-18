#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>

#include "simjson_object.h"
#include "simjson_type.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//todo 负载因子太大时怎么办？
//todo solution 1. 由用户在decode时传入BUCKET_SIZE参考值
//todo solution 2. 负载因子过大时，创建一个更大的哈希表，然后重哈希
//todo solution 3. decode时若检测到是object，先遍历object，获取键值对数量，根据数量来决定BUCKET_SIZE
const static uint8_t BUCKET_SIZE = 64;

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

struct SimjsonObjectItem {
    struct SimjsonObjectItem *next;
    char *key;
    size_t key_length;
    void *json_struct;
};

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PRIVATE SimjsonObjectItem *simjson_object_item_new(const char *key, size_t key_length, void *json_struct) {
    SimjsonObjectItem *item = malloc(sizeof(SimjsonObjectItem));
    if (item == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    item->key = malloc(sizeof(key_length + 1));
    if (item->key == NULL) {
        DEBUG_INFO(strerror(errno));
        free(item);
        return NULL;
    }
    memcpy(item->key, key, key_length);
    item->key[key_length] = '\0';
    item->key_length = key_length;
    item->json_struct = json_struct;
    item->next = NULL;

    return item;
}

SIMJSON_PRIVATE void simjson_object_item_free(SimjsonObjectItem *item) {
    simjson_free_json_struct(item->json_struct);
    free(item->key);
    free(item);
}

//djb2
SIMJSON_PRIVATE unsigned long hash_func(unsigned char *str, size_t length) {
    unsigned long hash = 5381;
    int c;
    size_t cur_length = length;

    while (cur_length--) {
        c = *str++;
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PUBLIC SimjsonObject *simjson_object_new(size_t bucket_size) {
    if (bucket_size == 0) {
        bucket_size = BUCKET_SIZE;
    }

    SimjsonObject *object = malloc(sizeof(SimjsonObject));
    if (object == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    //不能用malloc
    object->buckets = calloc(bucket_size, sizeof(struct SimjsonObjectItem *));
    if (object->buckets == NULL) {
        DEBUG_INFO(strerror(errno));
        free(object);
        return NULL;
    }

    object->bucket_size = bucket_size;
    object->item_size = 0;
    object->type = SIMJSON_OBJECT_TYPE;

    return object;
}

SIMJSON_PUBLIC void simjson_object_free(SimjsonObject *object) {
    size_t remaining_items = object->item_size;
    size_t cur_bucket_index = 0;
    SimjsonObjectItem *cur_item = NULL;
    SimjsonObjectItem *next_item = NULL;

    while (remaining_items) {
        cur_item = object->buckets[cur_bucket_index];
        while (cur_item != NULL) {
            next_item = cur_item->next;
            simjson_object_item_free(cur_item);
            remaining_items--;
            cur_item = next_item;
        }
        //cur_bucket_index应该不会越界，因为有检查remaining_items
        cur_bucket_index++;
    }

    free(object->buckets);
    free(object);
}

SIMJSON_PUBLIC bool simjson_object_add(SimjsonObject *object, const char *key, size_t key_length, void *json_struct) {
    if (object == NULL) {
        DEBUG_INFO("object is NULL");
        return false;
    }

    if (key == NULL || key_length == 0) {
        DEBUG_INFO("key is NULL or empty");
        return false;
    }

    if (json_struct == NULL) {
        DEBUG_INFO("json_struct is NULL");
        return false;
    }

    //todo 直接调用simjson_object_get的话，计算了两次hash
    if (simjson_object_get(object, key, key_length) != NULL) {
        DEBUG_INFO("key already exists");
        return false;
    }

    SimjsonObjectItem *item = simjson_object_item_new(key, key_length, json_struct);
    if (item == NULL) {
        return false;
    }

    unsigned long key_hash = hash_func((unsigned char *) key, key_length);
    size_t index = key_hash % (object->bucket_size - 1);
    SimjsonObjectItem *bucket = object->buckets[index];
    item->next = bucket;
    object->buckets[index] = item;

    object->item_size++;

    return true;
}

SIMJSON_PUBLIC void *simjson_object_get(SimjsonObject *object, const char *key, size_t key_length) {
    if (object == NULL) {
        DEBUG_INFO("object is NULL");
        return NULL;
    }

    if (key == NULL || key_length == 0) {
        DEBUG_INFO("key is NULL or empty");
        return NULL;
    }

    unsigned long hash = hash_func((unsigned char *) key, key_length);
    size_t index = hash % (object->bucket_size - 1);
    SimjsonObjectItem *item = object->buckets[index];

    if (item != NULL) {
        while (item) {
            if (key_length == item->key_length && strncmp(item->key, key, key_length) == 0) {
                return item->json_struct;
            }
            else {
                item = item->next;
            }
        }
    }

//    DEBUG_INFO("key does not exists");
    return NULL;
}

SIMJSON_PUBLIC bool simjson_object_delete(SimjsonObject *object, const char *key, size_t key_length) {
    if (object == NULL) {
        DEBUG_INFO("object is NULL");
        return false;
    }

    if (key == NULL || key_length == 0) {
        DEBUG_INFO("key is NULL or empty");
        return false;
    }

    unsigned long hash = hash_func((unsigned char *) key, key_length);
    size_t index = hash % (object->bucket_size - 1);
    SimjsonObjectItem *cur_item = object->buckets[index];
    SimjsonObjectItem *prev_item = NULL;

    if (cur_item != NULL) {
        while (cur_item) {
            if (key_length == cur_item->key_length && strncmp(cur_item->key, key, key_length) == 0) {
                if (prev_item != NULL) {
                    prev_item->next = cur_item->next;
                }
                else {
                    object->buckets[index] = cur_item->next;
                }
                simjson_object_item_free(cur_item);
                object->item_size--;
                return true;
            }
            else {
                prev_item = cur_item;
                cur_item = cur_item->next;
            }
        }
    }

    DEBUG_INFO("key does not exists");
    return false;
}

SIMJSON_PUBLIC SimjsonObjectIterator *simjson_object_iterator_new(SimjsonObject *object) {
    if (object == NULL) {
        DEBUG_INFO("object is NULL");
        return NULL;
    }

    SimjsonObjectIterator *iterator = malloc(sizeof(SimjsonObjectIterator));
    if (iterator == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    iterator->object = object;
    iterator->cur_bucket_index = 0;
    iterator->cur_item_index = 0;
    iterator->cur_item = NULL;

    return iterator;
}

SIMJSON_PUBLIC void simjson_object_iterator_free(SimjsonObjectIterator *iterator) {
    free(iterator);
}

SIMJSON_PUBLIC bool simjson_object_iterator_has_next(SimjsonObjectIterator *iterator) {
    if (iterator == NULL) {
        DEBUG_INFO("iterator is NULL");
        return false;
    }
    return iterator->cur_item_index < iterator->object->item_size;
}

SIMJSON_PUBLIC void *simjson_object_iterator_next(SimjsonObjectIterator *iterator, char **key, size_t *key_length) {
    if (!simjson_object_iterator_has_next(iterator)) {
        return NULL;
    }

    SimjsonObjectItem *cur_item = iterator->cur_item;
    SimjsonObject *object = iterator->object;
    while (cur_item == NULL) {
        //cur_bucket_index应该不会越界，因为检查了simjson_object_iterator_has_next
        //除非在迭代过程中删除键值对
        iterator->cur_bucket_index++;
        cur_item = object->buckets[iterator->cur_bucket_index];
    }
    iterator->cur_item = cur_item->next;
    iterator->cur_item_index++;

    if (key != NULL) {
        *key = cur_item->key;
    }
    if (key_length != NULL) {
        *key_length = cur_item->key_length;
    }
    return cur_item->json_struct;
}
