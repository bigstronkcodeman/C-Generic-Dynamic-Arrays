#include "dynarray.h"

void __ar_init(struct DynArr* da, size_t stride, size_t argc, ...) {
    va_list ap;
    va_start(ap, argc);

    da->cap = DEFAULT_CAP;
    da->len = 0;
    da->stride = stride;
    da->data = malloc(sizeof(*da->data) * stride);

    if (argc == 0) {
        da->copy = NULL;
        da->destroy = NULL;
    }
    else if (argc == 2) {
        da->copy = va_arg(ap, void*(*)(void*));
        da->destroy = va_arg(ap, void(*)(void*));
    }
    else {
        exit(EXIT_FAILURE);
    }
}

void __ar_del(struct DynArr* da) {
    if (da->destroy) {
        for (size_t i = 0; i < da->len; i++) {
            da->destroy(da->data + da->stride * i);
        }
    }
    free(da->data);
    da->cap = 0;
    da->len = 0;
    da->stride = 0;
}

void __ar_push(struct DynArr* da, void* item) {
    if (da->len >= da->cap) {
        __ar_grow(da);
    }
    if (da->copy) {
        void* new_item = da->copy(item);
        memcpy(da->data + da->stride * da->len++, new_item, da->stride);
        free(new_item);
    }
    else {
        memcpy(da->data + da->stride * da->len++, item, da->stride);
    }
}

void __ar_pop(struct DynArr* da) {
    if (da->len < da->cap/GROWTH_FACTOR) {
        __ar_shrink(da);
    }
    da->len--;
}

void __ar_grow(struct DynArr* da) {
    da->cap *= GROWTH_FACTOR;
    void* new_data = malloc(da->stride * da->cap);
    if (new_data) {
        memcpy(new_data, da->data, da->stride * da->len);
        free(da->data);
        da->data = new_data;
    }
    else {
        exit(EXIT_FAILURE);
    }
}

void __ar_shrink(struct DynArr* da) {
    if (da->cap > 1) {
        da->cap /= GROWTH_FACTOR;
        void* new_data = malloc(da->stride * da->cap);
        if (new_data) {
            if (da->copy) {
                for (size_t i = 0; i < da->len; i++) {
                    void* new_item = da->copy(da->data + da->stride * i);
                    memcpy(da->data, new_item, da->stride);
                    free(new_item);
                }
            }
            else {
                memcpy(new_data, da->data, da->stride * da->len);
            }
        }
        else {
            exit(EXIT_FAILURE);
        }
    }
    else {
        exit(EXIT_FAILURE);
    }
}

void* __ar_get(struct DynArr* da, size_t i) {
    if (i >= da->len) {
        exit(EXIT_FAILURE);
    }
    return da->data + da->stride * i;
}

// todo
void __ar_insert(struct DynArr* da, void* item, size_t i) {
    if (da->len >= da->cap) {
        __ar_grow(da);
    }
    if (i < da->len) {

    }
    else {
        exit(EXIT_FAILURE);
    }
}







void* copyda(void* da) {
    struct DynArr* daptr = da;
    struct DynArr* new_da = malloc(sizeof(*new_da));
    *new_da = *daptr;
    void* new_data = malloc(daptr->cap * daptr->stride);
    if (new_data) {
        memcpy(new_data, daptr->data, daptr->stride * daptr->len);
        new_da->data = new_data;
    }
    else {
        exit(EXIT_FAILURE);
    }
    return new_da;
}

void destroyda(void* da) {
    struct DynArr* daptr = da;
    free(daptr->data);
    daptr->cap = 0;
    daptr->len = 0;
    daptr->stride = 0;
    daptr->data = NULL;
    daptr->copy = NULL;
    daptr->destroy = NULL;
}

void* copyouterda(void* da) {
    struct DynArr* daptr = da;
    struct DynArr* new_da =  malloc(sizeof(*new_da));
    *new_da = *daptr;
    void* new_data = malloc(daptr->stride * daptr->cap);
    if (new_data) {
        for(size_t i = 0; i < daptr->len; i++) {
            void* copy = copyda(daptr->data + daptr->stride * i);
            memcpy(new_data + daptr->stride * i, copy, daptr->stride);
            free(copy);
        }
        new_da->data = new_data;
    }
    else {
        exit(EXIT_FAILURE);
    }
    return new_da;
}

void destroyouterda(void* da) {
    struct DynArr* daptr = da;
    for (size_t i = 0; i < daptr->len; i++) {
        destroyda(daptr->data + daptr->stride * i);
    }
    daptr->cap = 0;
    daptr->len = 0;
    daptr->stride = 0;
    daptr->data = NULL;
    daptr->copy = NULL;
    daptr->destroy = NULL;
}