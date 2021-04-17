#include <stdio.h>
#include "dynarray.h"

struct A {
    char x;
    float y;
    unsigned long long z;
};

int main() {
    struct DynArr da;
    struct DynArr da2;
    struct DynArr da3;
    struct DynArr da4;
    struct DynArr da5;

    ar_init(da, double);
    ar_init(da2, short);
    ar_init(da3, struct A);
    ar_init(da4, int[10][10]);
    ar_init(da5, struct DynArr, copyda, destroyda);

    ar_pushrv(da, 1454.2564);
    ar_pushrv(da, 94.333333);
    ar_pushrv(da, 87941.4);

    ar_pushrv(da2, 4987);
    ar_pushrv(da2, 165);
    ar_pushrv(da2, 6666);

    struct A a = {
            .x = 'X',
            .y = 3.14159f,
            .z = 1UL << 60
    };
    ar_push(da3, a); a.x = 'F';
    ar_push(da3, a); a.y = 2.71828f;
    ar_push(da3, a); a.z++;
    ar_push(da3, a);
    ar_pushrv(da3, ((struct A){ .x = 'W', .y = 1.1111111, .z = 49395 }));

    int arr[10][10] = { {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10},
                        {1,2,3,4,5,6,7,8,9,10} };
    ar_push(da4, arr); arr[4][0] = 111;
    ar_push(da4, arr); arr[8][1] = 222;
    ar_push(da4, arr); arr[2][2] = 333;

    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);
    ar_pop(da5);
    ar_pop(da5);
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);

    for (size_t i = 0; i < da.len; i++) {
        double* item = ar_get(da, i);
        printf("%lf\n", *item);
    } printf("\n");

    ar_del(da);

    for (size_t i = 0; i < da2.len; i++) {
        short* item = ar_get(da2, i);
        printf("%d\n", *item);
    } printf("\n");

    for (int i = 0; i < da3.len; i++) {
        struct A* item = ar_get(da3, i);
        printf("%c %f %llu\n", item->x, item->y, item->z);
    } printf("\n");

    for (size_t i = 0; i < da4.len; i++) {
        int (*item)[10][10] = ar_get(da4, i);
        for (size_t j = 0; j < 10; j++) {
            for (size_t k = 0; k < 10; k++) {
                printf("%d ", (*item)[j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    for (size_t i = 0; i < da5.len; i++) {
        struct DynArr* item = ar_get(da5, i);
        for (size_t j = 0; j < item->len; j++) {
            double* innr_item = ar_get(*item, j);
            printf("%lf ", *innr_item);
        }
        printf("\n");
    } printf("\n");

    ar_del(da2);
    ar_del(da3);
    ar_del(da4);
    ar_del(da5);

    struct DynArr da6;
    struct DynArr da7;
    struct DynArr da8;

    ar_init(da6, struct DynArr, copyouterda, destroyouterda);
    ar_init(da7, struct DynArr, copyda, destroyda);
    ar_init(da8, int);

    ar_pushrv(da8, 1); ar_pushrv(da8, 2); ar_pushrv(da8, 3);
    ar_push(da7, da8); ar_push(da6, da7); ar_pushrv(da8, 4);
    ar_push(da7, da8); ar_push(da6, da7); ar_pushrv(da8, 5);
    ar_push(da7, da8); ar_push(da6, da7);

    ar_pop(*(struct DynArr*)ar_get(da6, 2));

    for (size_t i = 0; i < da6.len; i++) {
        struct DynArr* item = ar_get(da6, i);
        for (size_t j = 0; j < item->len; j++) {
            struct DynArr* innr_item = ar_get(*item, j);
            for (size_t k = 0; k < innr_item->len; k++) {
                int* innr_innr_item = ar_get(*innr_item, k);
                printf("%d ", *innr_innr_item);
            }
            printf("\n");
        }
        printf("\n");
    }

    for (size_t i = 0; i < da6.len; i++) {
        struct DynArr* item = ar_get(da6, i);
        ar_del(*item);
    }
    ar_del(da6);
    ar_del(da7);
    ar_del(da8);
    return 0;
}
