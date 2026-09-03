#ifndef PROGRAMME_H
#define PROGRAMME_H

#if defined(__cplusplus)
extern "C" {
#endif

#define COMMAND_IMPLEMENTATION
#include "command.h" // command_t, command_run_logged, command_run_async_logged

/**
 * @brief Representation of a programme.
 */
typedef struct
{
    size_t size;
    size_t capacity;
    logger_t *logger;
    command_t *commands;
} programme_t;

/**
 * @brief Construct a new programme.
 * @returns A new programme.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init();

/**
 * @brief Construct a new programme of a given capacity.
 * @param capacity Capacity to set to the programme.
 * @returns A new programme with a given capacity.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init_with_capacity(size_t capacity);

/**
 * @brief Construct a new programme with a logger.
 * @param logger Logger to set to the programme.
 * @return A new programme with a given logger.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init_with_logger(logger_t *logger);

/**
 * @brief Fully construct a new programme.
 * @param capacity Capacity to set to the programme.
 * @param logger Logger to set to the programme.
 * @returns A new programme with a given capacity and logger.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init_full(size_t capacity, logger_t *logger);

/**
 * @brief Set a given logger to the programme.
 * @param programme Programme to which to assign the logger.
 * @param logger Logger to set to the programme.
 */
void programme_set_logger(programme_t *programme, logger_t *logger);

/**
 * @brief Append a given command to a programme.
 * @param programme Programme to which to append a command.
 * @param command Command to append to a programme.
 * @exception If the programme can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void programme_append(programme_t *programme, command_t *command);

/**
 * @brief Obtain a command at a given index within the programme.
 * @param programme Programme to access.
 * @param index Index at which to access a command within the programme.
 * @returns The command stored within the programme at the given index.
 * @exception If the given index is greater than the size of the programme, an `OutOfRangeError` is printed to standard error and the programme exits.
 */
command_t *programme_at(programme_t *programme, size_t index);

/**
 * @brief Run a programme synchronously.
 * @param programme Programme to run.
 * @returns True is the programme is able to be ran, else false.
 */
bool programme_run(programme_t *programme);

/**
 * @brief Run a programme asynchronously.
 * @param programme Programme to run.
 * @returns An array of each of the processes returned from running the commands.
 */
process_array_t programme_run_async(programme_t *programme);

/**
 * @brief Resize the given programme by a factor of two.
 * @param programme Programme to resize.
 * @exception If the programme can not be resized, an `AllocationError` is printed to standard error and the programme exits.
 */
void programme_resize(programme_t *programme);

/**
 * @brief Resize the given programme by a given scaler value.
 * @param programme Programme to resize.
 * @param scaler Scaler value by which to resize the programme.
 * @exception If the programme can not be resized, an `AllocationError` is printed to standard error and the programme exits.
 */
void programme_resize_by(programme_t *programme, size_t scaler);

/**
 * @brief Deallocate the programme.
 * @param programme Programme to be deallocated.
 */
void programme_delete(programme_t *programme);

#if defined(__cplusplus)
}
#endif

#endif // PROGRAMME_H

#ifdef PROGRAMME_IMPLEMENTATION

#if defined(__cplusplus)
extern "C" {
#endif

#define PROGRAMME_INITIAL_CAPACITY 256

#define DEFAULT_LOGGER_NAME "programme"

/**
 * @brief Construct a new programme.
 * @returns A new programme.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init()
{
    return programme_init_with_capacity(PROGRAMME_INITIAL_CAPACITY);
}

/**
 * @brief Construct a new programme of a given capacity.
 * @param capacity Capacity to set to the programme.
 * @returns A new programme with a given capacity.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init_with_capacity(size_t capacity)
{
    logger_t *logger = logger_init(DEFAULT_LOGGER_NAME, LOG_DEBUG);
    return programme_init_full(capacity, logger);
}

/**
 * @brief Construct a new programme with a logger.
 * @param logger Logger to set to the programme.
 * @return A new programme with a given logger.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init_with_logger(logger_t *logger)
{
    return programme_init_full(PROGRAMME_INITIAL_CAPACITY, logger);
}

/**
 * @brief Fully construct a new programme.
 * @param capacity Capacity to set to the programme.
 * @param logger Logger to set to the programme.
 * @returns A new programme with a given capacity and logger.
 * @exception If the programme can not be allocated, an `AllocationError` is printed to standard error and the programme exits.
 */
programme_t programme_init_full(size_t capacity, logger_t *logger)
{
    command_t *commands = (command_t *)malloc(sizeof(command_t) * capacity);
    if (NULL == commands)
    {
        logger_log(logger, "AllocationError: Can not allocate a new programme.", LOG_CRITICAL);
        logger_close(logger);
        exit(1);
    }
    return (programme_t){
        .size = 0,
        .capacity = capacity,
        .logger = logger,
        .commands = commands
    };
}

/**
 * @brief Set a given logger to the programme.
 * @param programme Programme to which to assign the logger.
 * @param logger Logger to set to the programme.
 */
void programme_set_logger(programme_t *programme, logger_t *logger)
{
    programme->logger = logger;
}

/**
 * @brief Append a given command to a programme.
 * @param programme Programme to which to append a command.
 * @param command Command to append to a programme.
 * @exception If the programme can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void programme_append(programme_t *programme, command_t *command)
{
    if (programme->size >= programme->capacity)
    {
        programme_resize(programme);
    }
    programme->commands[programme->size++] = *command;
}

/**
 * @brief Obtain a command at a given index within the programme.
 * @param programme Programme to access.
 * @param index Index at which to access a command within the programme.
 * @returns The command stored within the programme at the given index.
 * @exception If the given index is greater than the size of the programme, an `OutOfRangeError` is printed to standard error and the programme exits.
 */
command_t *programme_at(programme_t *programme, size_t index)
{
    if (index >= programme->size)
    {
        logger_log(programme->logger, "OutOfRangeError: Can not access array outside of bounds.\n", LOG_CRITICAL);
        programme_delete(programme);
        exit(1);
    }
    return &programme->commands[index];
}

/**
 * @brief Run a programme synchronously.
 * @param programme Programme to run.
 * @returns True is the programme is able to be ran, else false.
 */
bool programme_run(programme_t *programme)
{
    for (size_t i = 0; i < programme->size; ++i)
    {
        if (!command_run_logged(&programme->commands[i], programme->logger)) return false;
    }
    return true;
}

/**
 * @brief Run a programme asynchronously.
 * @param programme Programme to run.
 * @returns An array of each of the processes returned from running the commands.
 */
process_array_t programme_run_async(programme_t *programme)
{
    process_array_t processes = process_array_init_with_capacity(programme->size);
    for (size_t i = 0; i < programme->size; ++i)
    {
        process_array_append(&processes, command_run_async_logged(&programme->commands[i], programme->logger));
    }
    return processes;
}

/**
 * @brief Resize the given programme by a factor of two.
 * @param programme Programme to resize.
 * @exception If the programme can not be resized, an `AllocationError` is printed to standard error and the programme exits.
 */
void programme_resize(programme_t *programme)
{
    programme_resize_by(programme, 2);
}

/**
 * @brief Resize the given programme by a given scaler value.
 * @param programme Programme to resize.
 * @param scaler Scaler value by which to resize the programme.
 * @exception If the programme can not be resized, an `AllocationError` is printed to standard error and the programme exits.
 */
void programme_resize_by(programme_t *programme, size_t scaler)
{
    programme->capacity *= scaler;
    programme->commands = (command_t *)realloc(programme->commands, sizeof(command_t) * programme->capacity);
    if (NULL == programme->commands)
    {
        logger_log(programme->logger, "AllocationError: Can not resize the command array.\n", LOG_CRITICAL);
        programme_delete(programme);
        exit(1);
    }
}

/**
 * @brief Deallocate the programme.
 * @param programme Programme to be deallocated.
 */
void programme_delete(programme_t *programme)
{
    logger_delete(programme->logger);
    if (!programme->commands) return;
    for (size_t i = 0; i < programme->size; ++i)
    {
        command_delete(&programme->commands[i]);
    }
    free(programme->commands);
    programme->commands = NULL;
}

#if defined(__cplusplus)
}
#endif

#endif // PROGRAMME_IMPLEMENTATION