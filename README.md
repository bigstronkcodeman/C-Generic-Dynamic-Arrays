# C-Generic-Dynamic-Arrays
A construct that supports dynamically resizing arrays of generic types in C. Emulates the behavior of something like C++'s std::vector.

My implementation of a generic dynamic array in C. Supports arbitrarily complex types via the addition of optional user-defined copy constructors and destructors. Some example usage:

```
#include <stdio.h>
#include "dynarray.h"
 
// example struct
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

    // initialize the containers with their respective underlying types
    ar_init(da, double);
    ar_init(da2, short);
    ar_init(da3, struct A);
    ar_init(da4, int[10][10]);
    ar_init(da5, struct DynArr, copyda, destroyda); // copy constructor and destructor optional, for memory-owning types

    // push some rvalues into arrays
    ar_pushrv(da, 1454.2564);
    ar_pushrv(da, 94.333333);
    ar_pushrv(da, 87941.4);
    ar_pushrv(da, 5349.15344);    
    ar_pushrv(da2, 4987);
    ar_pushrv(da2, 165);
    ar_pushrv(da2, 6666);

    // insert and remove some items from da
    ar_insertrv(da, 567.765, 1);
    ar_insertrv(da, 1234.5678, 2);
    ar_remove(da, 5);
    ar_remove(da, 0);

    // initialize a struct and push it onto an array
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

    // initialize and add 2D arrays to a dynamic array
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
    ar_push(da4, arr);

    // push removed values back onto an array
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);
    ar_push(da5, da); ar_pushrv(da, *(double*)ar_get(da, da.len-1) + 1);

    // iterate over and print a dynamic array of floats
    for (size_t i = 0; i < da.len; i++) {
        double* item = ar_get(da, i);
        printf("%lf\n", *item);
    } printf("\n");

    // free a dynamic array
    ar_del(da);

    // iterate over and print a dynamic array of shorts
    for (size_t i = 0; i < da2.len; i++) {
        short* item = ar_get(da2, i);
        printf("%d\n", *item);
    } printf("\n");

    // iterate over and print a dynamic array of custom type
    for (int i = 0; i < da3.len; i++) {
        struct A* item = ar_get(da3, i);
        printf("%c %f %llu\n", item->x, item->y, item->z);
    } printf("\n");

    // iterate over and print 2D arrays stored in dynamic array
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

    // iterate over and print a nested dynamic array (a dynamic array of dynamic arrays)
    for (size_t i = 0; i < da5.len; i++) {
        struct DynArr* item = ar_get(da5, i);
        for (size_t j = 0; j < item->len; j++) {
            double* innr_item = ar_get(*item, j);
            printf("%lf ", *innr_item);
        }
        printf("\n");
    } printf("\n");

    // free some more arrays
    ar_del(da2);
    ar_del(da3);
    ar_del(da4);
    ar_del(da5);
    
    struct DynArr da6;
    struct DynArr da7;
    struct DynArr da8;

    // initialize new dynamic arrays of memory-owning types
    ar_init(da6, struct DynArr, copyouterda, destroyouterda);
    ar_init(da7, struct DynArr, copyda, destroyda);
    ar_init(da8, int);

    // push and pop some values
    ar_pushrv(da8, 1); ar_pushrv(da8, 2); ar_pushrv(da8, 3);
    ar_push(da7, da8); ar_push(da6, da7); ar_pushrv(da8, 4);
    ar_push(da7, da8); ar_push(da6, da7); ar_pushrv(da8, 5);
    ar_pop(da7);
    ar_push(da7, da8); ar_push(da6, da7);
    ar_remove(da6, 1);

    // iterate over and print a 3D dynamic array (dynamic array of dynamic arrays of dynamic arrays)
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

    // free the inner arrays
    for (size_t i = 0; i < da6.len; i++) {
        struct DynArr* item = ar_get(da6, i);
        ar_del(*item);
    }
    ar_del(da6);
    ar_del(da7);
    ar_del(da8);


    return 0;
}
```
