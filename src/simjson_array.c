#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>

#include "simjson_array.h"
#include "log.h"

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

struct SimjsonArrayItem {
    struct SimjsonArrayItem *next;
    struct SimjsonArrayItem *prev;
    void *json_struct;
};

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PRIVATE SimjsonArrayItem *simjson_array_item_new(void *json_struct) {
    SimjsonArrayItem *item = malloc(sizeof(SimjsonArrayItem));
    if (item == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    item->json_struct = json_struct;
    item->prev = NULL;
    item->next = NULL;

    return item;
}

SIMJSON_PRIVATE void simjson_array_item_free(SimjsonArrayItem *item) {
    simjson_free_json_struct(item->json_struct);
    free(item);
}

/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

SIMJSON_PUBLIC SimjsonArray *simjson_array_new() {
    SimjsonArray *array = malloc(sizeof(SimjsonArray));
    if (array == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    array->head = simjson_array_item_new(NULL);
    if (array->head == NULL) {
        free(array);
        return NULL;
    }

    array->tail = simjson_array_item_new(NULL);
    if (array->tail == NULL) {
        free(array->head);
        free(array);
        return NULL;
    }

    array->head->next = array->tail;
    array->tail->prev = array->head;
    array->size = 0;
    array->type = SIMJSON_ARRAY_TYPE;

    return array;
}

SIMJSON_PUBLIC void simjson_array_free(SimjsonArray *array) {
    if (array == NULL) {
        DEBUG_INFO("array is NULL");
        return;
    }

    SimjsonArrayItem *cur_item = array->head;
    SimjsonArrayItem *next_item = NULL;
    while (cur_item != NULL) {
        next_item = cur_item->next;
        simjson_array_item_free(cur_item);
        cur_item = next_item;
    }
    free(array);
}

SIMJSON_PUBLIC bool simjson_array_insert(SimjsonArray *array, void *json_struct, size_t index) {
    if (array == NULL) {
        DEBUG_INFO("array is NULL");
        return false;
    }

    if (json_struct == NULL) {
        DEBUG_INFO("json_struct is NULL");
    }

    if (index < 0 || index > array->size) {
        DEBUG_INFO("index out of range");
        return false;
    }

    SimjsonArrayItem *item = simjson_array_item_new(json_struct);
    if (item == NULL) {
        return false;
    }

    SimjsonArrayItem *cur_item;
    if (index == array->size) {
        cur_item = array->tail->prev;
    }
    else {
        cur_item = array->head;
        size_t cur_index = index;
        while (cur_index--) {
            cur_item = cur_item->next;
        }
    }

    item->prev = cur_item;
    item->next = cur_item->next;
    cur_item->next->prev = item;
    cur_item->next = item;

    array->size++;

    return true;
}

SIMJSON_PUBLIC void *simjson_array_get(SimjsonArray *array, size_t index) {
    if (array == NULL) {
        DEBUG_INFO("array is NULL");
        return NULL;
    }

    if (index < 0 || index >= array->size) {
        DEBUG_INFO("index out of range");
        return NULL;
    }

    if (index == array->size - 1) {
        return array->tail->prev->json_struct;
    }

    SimjsonArrayItem *cur_item = array->head->next;
    size_t cur_index = index;
    while (cur_index--) {
        cur_item = cur_item->next;
    }
    return cur_item->json_struct;
}

SIMJSON_PUBLIC bool simjson_array_delete(SimjsonArray *array, size_t index) {
    if (array == NULL) {
        DEBUG_INFO("array is NULL");
        return false;
    }

    if (index < 0 || index >= array->size) {
        DEBUG_INFO("index out of range");
        return false;
    }

    SimjsonArrayItem *cur_item;
    if (index == array->size - 1) {
        cur_item = array->tail->prev;
    }
    else {
        cur_item = array->head->next;
        size_t cur_index = index;
        while (cur_index--) {
            cur_item = cur_item->next;
        }
    }

    cur_item->prev->next = cur_item->next;
    cur_item->next->prev = cur_item->prev;
    simjson_array_item_free(cur_item);
    array->size--;

    return true;
}

SIMJSON_PUBLIC SimjsonArrayIterator *simjson_array_iterator_new(SimjsonArray *array, size_t start_index) {
    if (array == NULL) {
        DEBUG_INFO("array is NULL");
        return NULL;
    }

    if (start_index < 0 || (start_index >= array->size)) {
        DEBUG_INFO("start_index out of range");
        return NULL;
    }

    SimjsonArrayIterator *iterator = malloc(sizeof(SimjsonArrayIterator));
    if (iterator == NULL) {
        DEBUG_INFO(strerror(errno));
        return NULL;
    }

    iterator->cur_item = array->head->next;
    iterator->size = array->size;
    iterator->cur_index = 0;
    while (iterator->cur_index != start_index) {
        iterator->cur_item = iterator->cur_item->next;
        iterator->cur_index++;
    }

    return iterator;
}

SIMJSON_PUBLIC void simjson_array_iterator_free(SimjsonArrayIterator *iterator) {
    free(iterator);
}

SIMJSON_PUBLIC bool simjson_array_iterator_has_next(SimjsonArrayIterator *iterator) {
    if (iterator == NULL) {
        DEBUG_INFO("iterator is NULL");
        return false;
    }
    return iterator->cur_index <= iterator->size - 1;
}

SIMJSON_PUBLIC void *simjson_array_iterator_next(SimjsonArrayIterator *iterator, size_t *index) {
    if (iterator == NULL) {
        DEBUG_INFO("iterator is NULL");
        return false;
    }

    if (iterator->cur_item != NULL) {
        void *json_struct = iterator->cur_item->json_struct;
        iterator->cur_item = iterator->cur_item->next;
        if (index != NULL) {
            *index = iterator->cur_index;
        }
        iterator->cur_index++;
        return json_struct;
    }
    return NULL;
}


