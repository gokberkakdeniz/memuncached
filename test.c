#include "vector.h"
#include <stdio.h>

int main(int argc, char const* argv[])
{
    vector_t* v = vector_create(10);

    int x = 66;

    printf("# v->size=%d, v->count=%d\n", v->size, v->count);
    for (int i = 0; i < v->count; i++)
        printf("v[%d] = %x, *v[%d] = %d\n", i, vector_at(v, i), i, *(int*)vector_at(v, i));

    vector_insert(v, (void*)&x);
    printf("# v->size=%d, v->count=%d\n", v->size, v->count);
    for (int i = 0; i < v->count; i++)
        printf("v[%d] = %x, *v[%d] = %d\n", i, vector_at(v, i), i, *(int*)vector_at(v, i));

    vector_remove(v, 0);
    printf("# v->size=%d, v->count=%d\n", v->size, v->count);
    for (int i = 0; i < v->count; i++)
        printf("v[%d] = %x, *v[%d] = %d\n", i, vector_at(v, i), i, *(int*)vector_at(v, i));

    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    for (size_t i = 0; i < 11; i++) {
        vector_insert(v, (void*)&arr[i]);
    }
    printf("# v->size=%d, v->count=%d\n", v->size, v->count);
    for (int i = 0; i < v->count; i++)
        printf("v[%d] = %x, *v[%d] = %d\n", i, vector_at(v, i), i, *(int*)vector_at(v, i));

    vector_remove(v, 0);
    vector_remove(v, 9);
    vector_remove(v, 5);
    vector_remove(v, 20);

    printf("# v->size=%d, v->count=%d\n", v->size, v->count);
    for (int i = 0; i < v->count; i++)
        printf("v[%d] = %x, *v[%d] = %d\n", i, vector_at(v, i), i, *(int*)vector_at(v, i));

    vector_destroy(v);

    return 0;
}
