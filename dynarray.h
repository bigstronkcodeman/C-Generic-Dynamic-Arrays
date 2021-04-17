#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define DEFAULT_CAP 1
#define GROWTH_FACTOR 2

struct DynArr {
    void* data;
    size_t len;
    size_t cap;
    size_t stride;
    void* (*copy)(void*);
    void (*destroy)(void*);
};

void __ar_init(struct DynArr* da, size_t stride, size_t argc, ...);
void __ar_del(struct DynArr* da);
void __ar_push(struct DynArr* da, void* item);
void __ar_pop(struct DynArr* da);
void __ar_grow(struct DynArr* da);
void __ar_shrink(struct DynArr* da);
void* __ar_get(struct DynArr* da, size_t i);
void __ar_insert(struct DynArr* da, void* item, size_t i);

void* copyda(void* da);
void destroyda(void* da);
void* copyouterda(void* da);
void destroyouterda(void* da);

#define FOURTH_ARG(a1, a2, a3, a4, ...) a4
#define NUM_ARGS(...) FOURTH_ARG(placeholder, ##__VA_ARGS__, 2, 1, 0)

#define ar_init(da, type, ...) __ar_init(&da, sizeof(type), NUM_ARGS(__VA_ARGS__), ## __VA_ARGS__)
#define ar_del(da) __ar_del(&da)
#define ar_push(da, item) __ar_push(&da, &item)
#define ar_pop(da) __ar_pop(&da)
#define ar_pushrv(da, rv)         \
    do {                          \
        __auto_type tmp = rv;     \
        __ar_push(&da, &tmp);     \
    } while(0)
#define ar_get(da, i) __ar_get(&da, i)
#define ar_insert(da, item, i) __ar_insert(&da, &item, i)


