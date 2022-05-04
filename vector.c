#include "vector.h"

vector_t* vector_create(size_t size)
{
    vector_t* vector_ptr = (vector_t*)malloc(sizeof(vector_t));

    vector_ptr->array = (void**)calloc(size, sizeof(void*));
    vector_ptr->size = size;
    vector_ptr->count = 0;

    if (pthread_mutex_init(&vector_ptr->lock, NULL) != 0) {
        free(vector_ptr);
        return NULL;
    }

    return vector_ptr;
}

void vector_destroy(vector_t* v)
{
    pthread_mutex_destroy(&v->lock);
    free(v->array);
    free(v);
}

void* vector_at_unsafe(vector_t* v, size_t index)
{
    void* result = NULL;

    if (index < v->size) {
        result = v->array[index];
    }

    return result;
}

void* vector_at(vector_t* v, size_t index)
{
    vector_lock(v);
    void* result = vector_at_unsafe(v, index);
    vector_unlock(v);
    return result;
}

bool vector_insert_unsafe(vector_t* v, void* element)
{
    if (v->size == v->count) {
        v->size *= 2;
        void* new_array = realloc(v->array, v->size * sizeof(void*));
        if (new_array == NULL) {
            return false;
        } else {
            v->array = new_array;
        }
    }

    v->array[v->count++] = element;

    return true;
}

bool vector_insert(vector_t* v, void* element)
{
    vector_lock(v);
    bool result = vector_insert_unsafe(v, element);
    vector_unlock(v);
    return result;
}

void* vector_remove_unsafe(vector_t* v, size_t index)
{
    if (index >= v->size) {
        return NULL;
    }

    void* eptr = v->array[index];

    if (eptr == NULL) {
        return NULL;
    }

    v->array[index] = NULL;
    --v->count;

    if (v->count > 0) {
        v->array[index] = v->array[v->count];
        v->array[v->count] = NULL;
    }

    return eptr;
}

void* vector_remove(vector_t* v, size_t index)
{
    vector_lock(v);
    void* eptr = vector_remove_unsafe(v, index);
    vector_unlock(v);
    return eptr;
}

void vector_lock(vector_t* v)
{
    pthread_mutex_lock(&v->lock);
}

void vector_unlock(vector_t* v)
{
    pthread_mutex_unlock(&v->lock);
}

int vector_timed_lock(vector_t* v, size_t sec)
{
    struct timespec timeout_time;
    clock_gettime(CLOCK_REALTIME, &timeout_time);
    timeout_time.tv_sec += sec;

    return pthread_mutex_timedlock(&v->lock, &timeout_time);
}