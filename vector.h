#ifndef H_VECTOR
#define H_VECTOR

/**
 * @file vector.h
 * @author Gokberk Akdeniz
 * @brief Thread-safe dynamic array
 * @version 0.1
 * @date 2022-05-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct vector {
    void** array;
    size_t size;
    size_t count;
    pthread_mutex_t lock;
} vector_t;

/**
 * @brief constructor
 * 
 * @param size inital size
 * @return vector_t* instance
 */
vector_t* vector_create(size_t size);

/**
 * @brief destructor
 * 
 * @param v instance
 */
void vector_destroy(vector_t* v);

/**
 * @brief locks (usefull when traversing)
 * 
 * @param v instance
 */
void vector_lock(vector_t* v);

/**
 * @brief locks but with timeout (usefull when traversing)
 *  
 * @param v instance
 * @param sec timeout (sec)
 * @return 0 if success, otherwise error code
 */
int vector_timed_lock(vector_t* v, size_t sec);

/**
 * @brief unlocks (usefull when traversing)
 * 
 * @param v instance
 */
void vector_unlock(vector_t* v);

/**
 * @brief returns indexth element
 * 
 * @param v instance
 * @param index index
 * @return void* value
 */
void* vector_at(vector_t* v, size_t index);

/**
 * @brief returns indexth element (does not lock)
 * 
 * @param v instance
 * @param index index
 * @return void* value
 */
void* vector_at_unsafe(vector_t* v, size_t index);

/**
 * @brief removes indexth element
 * 
 * @param v instance
 * @param index index
 * @return void* value
 */
void* vector_remove(vector_t* v, size_t index);

/**
 * @brief removes indexth element (does not lock)
 * 
 * @param v instance
 * @param index index
 * @return void* value
 */
void* vector_remove_unsafe(vector_t* v, size_t index);

/**
 * @brief inserts new element
 * 
 * @param v instance
 * @param element an element to be inserted
 * @return bool
 */
bool vector_insert(vector_t* v, void* element);

/**
 * @brief inserts new element (does not lock
 * 
 * @param v instance
 * @param element an element to be inserted
 * @return bool
 */
bool vector_insert_unsafe(vector_t* v, void* element);

#endif