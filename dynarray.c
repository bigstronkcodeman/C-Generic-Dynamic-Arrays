#include "dynarray.h"

/*
 * Function: __ar_init
 * ------------------
 * Initializes a dynamic array.
 *
 * da: ptr to dynamic array to initialize
 * stride: the length of each element being stored by the array, in bytes
 * argc: the number of optional arguments passed for the variadic portion
 * ... : only defined for the cases when no 4th/5th arguments are passed or when two
 *      are. If two are passed, the first shall be a pointer to the copy function
 *      and the second shall be a pointer to the destroy function, both of which
 *      will be used by the array to manage heap memory.
 */
void __ar_init(struct DynArr* da, size_t stride, size_t argc, ...) {
    // initialize variadic parameter vector
    va_list ap;
    va_start(ap, argc);

    // default values, allocate memory buffer
    da->cap = DEFAULT_CAP;
    da->len = 0;
    da->stride = stride;
    da->data = malloc(sizeof(*da->data) * stride);

    if (argc == 0) {
        // we are storing types which do not own their own memory
        da->copy = NULL;
        da->destroy = NULL;
    }
    else if (argc == 2) {
        // we are storing types which own their own memory (such as in the case of nested dynamic arrays)
        da->copy = va_arg(ap, void*(*)(void*));
        da->destroy = va_arg(ap, void(*)(void*));
    }
    else {
        printf("ERROR: __ar_init received an invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Function: __ar_del
 * ------------------
 * Frees the memory owned by the dynamic array. If it is filled with elements that own
 * some block of heap memory, then iteratively destroy each one to free those extra blocks.
 *
 * da: ptr to dynamic array to be deleted/released
 */
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

/*
 * Function __ar_push
 * ------------------
 * Pushes an item onto the array, and expands the memory buffer to accommodate the extra item if necessary.
 * Makes sure to perform deep copies on individual items if they own memory.
 *
 * da: ptr to dynamic array to push item onto
 * item: ptr to item to push onto the array
 */
void __ar_push(struct DynArr* da, void* item) {
    if (da->len + 1 >= da->cap) {
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

/*
 * Function __ar_pop
 * ------------------
 * Remove the last element from the array. If the items being stored own heap memory,
 * then make sure to destroy it first. If not, then we need only decrement len.
 *
 * da: ptr to dynamic array from which the last element is being popped
 */
void __ar_pop(struct DynArr* da) {
    if (da->len > 0) {
        if (da->len <= da->cap / GROWTH_FACTOR) {
            __ar_shrink(da);
        }
        if (da->destroy) {
            da->destroy(da->data + da->stride * (da->len - 1));
        }
        da->len--;
    }
    else {
        printf("ERROR: __ar_pop called on array of length 0\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Function __ar_grow
 * ------------------
 * Resizes the array by a factor of GROWTH_FACTOR. This is essentially done by
 * allocating a new, bigger memory block and copying the elements over.
 *
 * da: ptr to dynamic array whose memory block we are expanding
 */
void __ar_grow(struct DynArr* da) {
    da->cap *= GROWTH_FACTOR;
    void* new_data = malloc(da->stride * da->cap);
    if (new_data) {
        if (da->copy) {
            for (size_t i = 0; i < da->len; i++) {
                void* new_item = da->copy(da->data + da->stride * i);
                da->destroy(da->data + da->stride * i);
                memcpy(new_data + da->stride * i, new_item, da->stride);
                free(new_item);
            }
        }
        else {
            memcpy(new_data, da->data, da->stride * da->len);
        }
        free(da->data);
        da->data = new_data;
    }
    else {
        printf("ERROR: __ar_grow failed to allocate a new buffer\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Function __ar_shrink
 * ------------------
 * Shrinks the array by a factor of GROWTH_FACTOR. This operation is the exact inverse of __ar_grow
 * and thusly works by allocating a new, smaller memory block and copying the items over.
 *
 * da: ptr to dynamic array that is being shrunk
 */
void __ar_shrink(struct DynArr* da) {
    if (da->cap > 1) {
        da->cap /= GROWTH_FACTOR;
        void* new_data = malloc(da->stride * da->cap);
        if (new_data) {
            if (da->copy) {
                for (size_t i = 0; i < da->len; i++) {
                    void* new_item = da->copy(da->data + da->stride * i);
                    da->destroy(da->data + da->stride * i);
                    memcpy(new_data + da->stride * i, new_item, da->stride);
                    free(new_item);
                }
            }
            else {
                memcpy(new_data, da->data, da->stride * da->len);
            }
            free(da->data);
            da->data = new_data;
        }
        else {
            printf("ERROR: __ar_shrink failed to allocate a new buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        printf("ERROR: __ar_shrink called on array of size <= 1\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Function __ar_get
 * -----------------
 * Get an item at a specific index in the array.
 *
 * da: ptr to dynamic array which is being accessed
 * i: index from which to retrieve item
 *
 * returns: a void pointer to the beginning of the portion of the memory block
 *          where the "i"th item resides. It is the user's responsibility to cast
 *          this pointer to the appropriate type lest they desire a silent and painful
 *          death for their program.
 */
void* __ar_get(struct DynArr* da, size_t i) {
    if (i < 0 || i >= da->len) {
        printf("ERROR: __ar_get called with invalid index\n");
        exit(EXIT_FAILURE);
    }
    return da->data + da->stride * i;
}

/*
 * Function __ar_insert
 * --------------------
 * Insert an item at the specified index in the array. Shifts items from indices i...n one to the right.
 *
 * da: ptr to dynamic array that is being inserted into
 * item: void ptr to the item to be inserted
 * i: index at which to insert the item
 */
void __ar_insert(struct DynArr* da, void* item, size_t i) {
    if (da->len + 1 >= da->cap) {
        __ar_grow(da);
    }
    if (i >= 0 && i < da->len) {
        for (void* ptr = da->data + da->stride * da->len; ptr > da->data + da->stride * i; ptr -= da->stride) {
           memcpy(ptr, ptr - da->stride, da->stride);
        }
        if (da->copy) {
            void* new_item = da->copy(item);
            memcpy(da->data + da->stride * i, new_item, da->stride);
            free(new_item);
        }
        else {
            memcpy(da->data + da->stride * i, item, da->stride);
        }
        da->len++;
    }
    else {
        printf("ERROR: __ar_insert called with invalid index\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Function __ar_remove
 * --------------------
 * Remove the item at the specified index from the array. Shift elements from indices i+1...n one to the left.
 *
 * da: ptr to dynamic array from which to remove item
 * i: index to remove item from
 */
void __ar_remove(struct DynArr* da, size_t i) {
    if (i >= 0 && i < da->len) {
        if (da->destroy) {
            da->destroy(da->data + da->stride * i);
        }
        for (void* ptr = da->data + da->stride * i; ptr + da->stride < da->data + da->stride * da->len; ptr += da->stride) {
            memcpy(ptr, ptr + da->stride, da->stride);
        }
        da->len--;
    }
    else {
        printf("ERROR: __ar_remove called with invalid index\n");
        exit(EXIT_FAILURE);
    }
}






// deep copy a dynamic array of some non-memory-owning type
void* copyda(void* da) {
    struct DynArr* daptr = da;
    struct DynArr* new_da = malloc(sizeof(*new_da));
    *new_da = *daptr;
    void* new_data = malloc(daptr->cap * daptr->stride);
    if (new_data) {
        memcpy(new_data, daptr->data, daptr->stride * daptr->len);
        new_da->data = new_data;
        return new_da;
    }
    else {
        exit(EXIT_FAILURE);
    }
}

// free a dynamic array of some non-memory-owning type
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

// deep copy a dynamic array of dynamic arrays
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
        return new_da;
    }
    else {
        exit(EXIT_FAILURE);
    }
}

// free a dynamic array of dynamic arrays
void destroyouterda(void* da) {
    struct DynArr* daptr = da;
    for (size_t i = 0; i < daptr->len; i++) {
        destroyda(daptr->data + daptr->stride * i);
    }
    free(daptr->data);
    daptr->cap = 0;
    daptr->len = 0;
    daptr->stride = 0;
    daptr->data = NULL;
    daptr->copy = NULL;
    daptr->destroy = NULL;
}
