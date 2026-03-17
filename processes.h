#ifndef PROCESSES_H
#define PROCESSES_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h> // size_t

#define PROCESS_IMPLEMENTATION
#include "process.h" // process_t

/**
 * @brief Representation of a process array.
 */
typedef struct
{
    size_t size;
    size_t capacity;
    process_t *processes;
} process_array_t;

/**
 * @brief Construct a new process array of a fixed capacity.
 * @returns A new process array of a fixed capacity.
 * @exception If the process array can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
process_array_t *process_array_init(void);

/**
 * @brief Construct a new process array of a given capacity.
 * @param capacity Capacity with which to construct a new process array.
 * @returns A new process array of a given capacity.
 * @exception If the process array can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
process_array_t *process_array_init_with_capacity(size_t capacity);

/**
 * @brief Append a given process to a process array.
 * @param processes Process array to which to append.
 * @param process Process to append to the array.
 * @exception If the process array can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void process_array_append(process_array_t *processes, process_t process);

/**
 * @brief Access a process array at a given index.
 * @param processes Process array to access.
 * @param index Index at which to access the process array.
 * @returns A pointer to a process within the given process array at the given index.
 * @exception If the index is greater than the size of the process array, an `OutOfRangeError` is is printed to standard error and the programme exits.
 */
process_t *process_array_at(process_array_t *processes, size_t index);

/**
 * @brief Wait for an array of processes.
 * @param processes Processes array on which to wait.
 * @returns True if each of the processes within the array can be successfully waited on, else false.
 */
bool process_array_wait(process_array_t *processes);

/**
 * @brief Resize a given process array by a factor of two.
 * @param processes Process array to resize.
 * @exception If the process array can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void process_array_resize(process_array_t *processes);

/**
 * @brief Resize a given process array by a given scaler value.
 * @param processes Process array to resize.
 * @param scaler Scaler value by which to resize the process array.
 * @exception If the process array can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void process_array_resize_by(process_array_t *processes, size_t scaler);

/**
 * @brief Deallocate the process array.
 * @param processes Process array to delete.
 */
void process_array_delete(process_array_t *processes);

#if defined(__cplusplus)
}
#endif

#endif // PROCESSES_H

#ifdef PROCESSES_IMPLEMENTATION

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h> // malloc, realloc free, exit, NULL
#include <stdio.h> // fprintf, stderr

/**
 * @brief Initial fixed capacity of a process array.
 */
#define PROCESS_ARRAY_INITIAL_CAPACITY 256

/**
 * @brief Construct a new process array of a fixed capacity.
 * @returns A new process array of a fixed capacity.
 * @exception If the process array can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
process_array_t *process_array_init(void)
{
    return process_array_init_with_capacity(PROCESS_ARRAY_INITIAL_CAPACITY);
}

/**
 * @brief Construct a new process array of a given capacity.
 * @param capacity Capacity with which to construct a new process array.
 * @returns A new process array of a given capacity.
 * @exception If the process array can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
process_array_t *process_array_init_with_capacity(size_t capacity)
{
    process_array_t *processes = (process_array_t *)malloc(sizeof(process_array_t));
    if (NULL == processes)
    {
        fprintf(stderr, "AllocationError: Can not allocate enough memory for a new process array.\n");
        exit(1);
    }
    processes->size = 0;
    processes->capacity = capacity;
    processes->processes = (process_t *)malloc(sizeof(process_t) * capacity);
    if (NULL == processes->processes)
    {
        fprintf(stderr, "AllocationError: Can not allocate enough memory for a new process array.\n");
        if (processes) free(processes);
        exit(1);
    }
    return processes;
}

/**
 * @brief Append a given process to a process array.
 * @param processes Process array to which to append.
 * @param process Process to append to the process array.
 * @exception If the process array can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void process_array_append(process_array_t *processes, process_t process)
{
    if (processes->size >= processes->capacity)
    {
        process_array_resize(processes);
    }
    processes->processes[processes->size++] = process;
}

/**
 * @brief Access a process array at a given index.
 * @param processes Process array to access.
 * @param index Index at which to access the process array.
 * @returns A pointer to a process within the given process array at the given index.
 * @exception If the index is greater than the size of the process array, an `OutOfRangeError` is is printed to standard error and the programme exits.
 */
process_t *process_array_at(process_array_t *processes, size_t index)
{
    if (index >= processes->size)
    {
        fprintf(stderr, "OutOfRangeError: Can not access array of size %zu at index %zu.\n", processes->size, index);
        process_array_delete(processes);
        exit(1);
    }
    return &processes->processes[index];
}

/**
 * @brief Wait for an array of processes.
 * @param processes Processes array on which to wait.
 * @returns True if each of the processes within the array can be successfully waited on, else false.
 */
bool process_array_wait(process_array_t *processes)
{
    bool success = true;
    for (size_t i = 0; i < processes->size; ++i)
    {
        success = process_wait(*process_array_at(processes, i)) && success;
    }
    return success;
}

/**
 * @brief Resize a given process array by a factor of two.
 * @param processes Process array to resize.
 * @exception If the process array can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void process_array_resize(process_array_t *processes)
{
    process_array_resize_by(processes, 2);
}

/**
 * @brief Resize a given process array by a given scaler value.
 * @param processes Process array to resize.
 * @param scaler Scaler value by which to resize the process array.
 * @exception If the process array can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void process_array_resize_by(process_array_t *processes, size_t scaler)
{
    processes->capacity *= scaler;
    processes->processes = (process_t *)realloc(processes->processes, processes->capacity);
    if (NULL == processes->processes)
    {
        fprintf(stderr, "AllocationError: Can not reallocate the process array.\n");
        process_array_delete(processes);
    }
}

/**
 * @brief Deallocate the process array.
 * @param processes Process array to delete.
 */
void process_array_delete(process_array_t *processes)
{
    if (!processes->processes) return;
    free(processes->processes);
    processes->processes = NULL;
    if (!processes) return;
    free(processes);
    processes = NULL;
}

#if defined(__cplusplus)
}
#endif

#endif // PROCESSES_IMPLEMENTATION