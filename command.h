#ifndef COMMAND_H
#define COMMAND_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h> // size_t
#include <stdbool.h> // bool

#define PROCESSES_IMPLEMENTATION
#include "processes.h" // process_t, process_wait, process_close

#define LOGGER_IMPLEMENTATION
#include "lib/c/logger.h" // logger_t, logger_log

/**
 * @brief Representation of a system command.
 */
typedef struct
{
    size_t size;
    size_t capacity;
    char *items;
} command_t;

/**
 * @brief Construct a new command with a fixed capacity.
 * @returns A new command with a fixed capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t command_init(void);

/**
 * @brief Construct a new command of a given capacity.
 * @param capacity Initial capacity of the command.
 * @returns A new command with a given capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t command_init_with_capacity(size_t capacity);

/**
 * @brief Append a null-terminated string to the command.
 * @param command Command to which to append.
 * @param string Null-terminated c-string to append to the command.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_append(command_t *command, const char *string);

/**
 * @brief Append a formatted string to a command.
 * @param command Command to which to append.
 * @param format Format string from which to append.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_appendf(command_t *command, const char *format, ...);

/**
 * @brief Access the command at a given index.
 * @param command Command to access.
 * @param index Index at which to access the command.
 * @returns A pointer to the character stored within the command at a given index.
 * @exception If the index is greater than or equal to the size of the command, an `IndexError` is printed to standard error and the programme exits.
 */
char *command_at(command_t *command, size_t index);

/**
 * @brief Obtain a null-terminated representation of the command.
 * @param command Command to represent.
 * @returns A null-terminated representation of the command.
 */
const char *command_data(command_t *command);

/**
 * @brief Obtain a non-null terminated representation of the command.
 * @param command Command to represent.
 * @returns A non-null terminated representation of the command.
 */
char *command_items(const command_t *command);

/**
 * @brief Run a command asynchronously.
 * @param command Command to run.
 * @returns A handle to the process of the command.
 * @returns If the command could not run, `INVALID_PROCESS` is returned.
 */
process_t command_run_async(command_t *command);

/**
 * @brief Run a command asynchronously logged.
 * @param command Command to run.
 * @param logger Logger with which to log the command.
 * @returns A handle to the process of the command.
 * @returns If the command could not run, `INVALID_PROCESS` is returned.
 */
process_t command_run_async_logged(command_t *command, const logger_t *logger);

/**
 * @brief Run a command synchronously.
 * @param command Command to run.
 * @returns True if the command has run successfully, else false.
 */
bool command_run(command_t *command);

/**
 * @brief Run a command synchronously logged.
 * @param command Command to run.
 * @param logger Logger with which to log the command.
 * @returns True if the command has run successfully, else false.
 */
bool command_run_logged(command_t *command, const logger_t *logger);

/**
 * @brief Resize the command by a factor of two.
 * @param command Command to resize.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_resize(command_t *command);

/**
 * @brief Resize the command by a given scalar value.
 * @param command Command to resize.
 * @param scalar Scalar value by which to resize the command.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_resize_by(command_t *command, size_t scalar);

/**
 * @brief Ensure that a command has a given capacity.
 * @param command Command to reserve.
 * @param capacity Minimum capacity required.
 * @exception If the command can not be reallocated, an `AllocationError`
 *            is printed to standard error and the programme exits.
 */
void command_reserve(command_t *command, size_t capacity);

/**
 * @brief Deallocate the command.
 * @param command Command to deallocate.
 */
void command_delete(command_t *command);

#if defined(__cplusplus)
}
#endif

#endif // COMMAND_H

#ifdef COMMAND_IMPLEMENTATION

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h> // malloc, realloc, free, exit, EXIT_FAILURE
#include <stdio.h> // fprintf, vsnprintf, sprintf
#include <string.h> // strlen
#include <stdarg.h> // va_list, va_start, va_end, va_copy
#include <stdint.h> // SIZE_MAX
#include <ctype.h> // isspace

#ifdef _WIN32
    #include <winnt.h> // HANDLE, DWORD
    #include <minwinbase.h> // ZeroMemory, FormatMessage, FORMAT_MESSAGE_FROM_SYSTEM, FORMAT_MESSAGE_IGNORE_INSERTS, LANG_USER_DEFAULT
    #include <winerror.h> // ERROR_MR_MID_NOT_FOUND
    #include <handleapi.h> // GetStdHandle, STD_OUTPUT_HANDLE, STD_INPUT_HANDLE, STD_ERROR_HANDLE, INVALID_HANDLE_VALUE
    #include <processthreadsapi.h> // STARTUPINFO, PROCESS_INFORMATION, CreateProcess
    #include <errhandlingapi.h> // GetLastError
#endif // _WIN32

#define BUFFER_IMPLEMENTATION
#include "lib/c/collections/buffer.h" // buffer_allocate, buffer_save, buffer_rewind

#define COMMAND_INITIAL_CAPACITY 256

/**
 * @brief Construct a new command with a fixed capacity.
 * @returns A new command with a fixed capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t command_init(void)
{
    return command_init_with_capacity(COMMAND_INITIAL_CAPACITY);
}

/**
 * @brief Construct a new command of a given capacity.
 * @param capacity Initial capacity of the command.
 * @returns A new command with a given capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t command_init_with_capacity(size_t capacity)
{
    char *commands = (char *)malloc(capacity * sizeof(char));
    if (NULL == commands)
    {
        fprintf(stderr, "AllocationError: Can not allocate enough memory for the array of commands.\n");
        exit(EXIT_FAILURE);
    }
    return (command_t)
    {
        .capacity = capacity,
        .size = 0,
        .items = commands
    };
}

/**
 * @brief Internal charactrer appendation function.
 * @param command Command to which to append.
 * @param item Item to append to the command.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
static void _command_append(command_t *command, char item)
{
    if (command->size >= command->capacity)
    {
        command_resize(command);
    }
    command->items[command->size++] = item;
}

/**
 * @brief Append a null-terminated string to the command.
 * @param command Command to which to append.
 * @param string Null-terminated c-string to append to the command.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_append(command_t *command, const char *string)
{
    size_t length = strlen(string);
    for (size_t i = 0; i < length; ++i)
    {
        _command_append(command, string[i]);
    }
}

/**
 * @brief Append a formatted string to a command.
 * @param command Command to which to append.
 * @param format Format string from which to append.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_appendf(command_t *command, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    va_list copy;
    va_copy(copy, arguments);
    int size = vsnprintf(NULL, 0, format, copy);
    va_end(copy);

    if (size < 0)
    {
        va_end(arguments);
        fprintf(stderr, "ValueError: 'vsnprintf' has failed.\n");
        command_delete(command);
        exit(EXIT_FAILURE);
    }
    else if ((size_t)size > SIZE_MAX - command->size - 1)
    {
        va_end(arguments);

        fprintf(stderr, "OverflowError: Command size has overflowed its type.\n");

        command_delete(command);
        exit(EXIT_FAILURE);
    }

    command_reserve(command, command->size + (size_t)size + 1);

    vsnprintf(command->items + command->size, (size_t)size + 1, format, arguments);

    va_end(arguments);

    command->size += (size_t)size;
}

/**
 * @brief Access the command at a given index.
 * @param command Command to access.
 * @param index Index at which to access the command.
 * @returns A pointer to the character stored within the command at a given index.
 * @exception If the index is greater than or equal to the size of the command, an `IndexError` is printed to standard error and the programme exits.
 */
char *command_at(command_t *command, size_t index)
{
    if (index >= command->size)
    {
        fprintf(stderr, "IndexError: Can not access array of size %zu at index %zu.\n", command->size, index);
        command_delete(command);
        exit(EXIT_FAILURE);
    }
    return &command->items[index];
}

/**
 * @brief Obtain a null-terminated representation of the command.
 * @param command Command to represent.
 * @returns A null-terminated representation of the command.
 */
const char *command_data(command_t *command)
{
    if (command->size >= command->capacity)
    {
        command_resize(command);
    }
    command->items[command->size] = '\0';
    return command->items;
}

/**
 * @brief Obtain a non-null terminated representation of the command.
 * @param command Command to represent.
 * @returns A non-null terminated representation of the command.
 */
char *command_items(const command_t *command)
{
    return command->items;
}

/**
 * @brief Resize the command by a factor of two.
 * @param command Command to resize.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_resize(command_t *command)
{
    command_resize_by(command, 2);
}

/**
 * @brief Resize the command by a given scalar value.
 * @param command Command to resize.
 * @param scalar Scalar value by which to resize the command.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_resize_by(command_t *command, size_t scalar)
{
    if (scalar < 2) return;
    else if (command->capacity > (SIZE_MAX / scalar))
    {
        fprintf(stderr, "OverflowError: The capacity has overflown its type.\n");
        command_delete(command);
        exit(EXIT_FAILURE);
    }
    command->capacity *= scalar;
    command->items = (char *)realloc(command->items, command->capacity * sizeof(char));
    if (NULL == command->items)
    {
        fprintf(stderr, "AllocationError: Can not reallocate the process array.\n");
        command_delete(command);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Ensure that a command has a given capacity.
 * @param command Command to reserve.
 * @param capacity Minimum capacity required.
 * @exception If the requested capacity overflows, an `OverflowError` is printed to standard error and the programme exits.
 * @exception If the command can not be reallocated, an `AllocationError` is printed to standard error and the programme exits.
 */
void command_reserve(command_t *command, size_t capacity)
{
    if (capacity <= command->capacity) return;

    if (capacity > SIZE_MAX / sizeof(*command->items))
    {
        fprintf(stderr, "OverflowError: The command capacity has overflowed its type.\n");
        command_delete(command);
        exit(EXIT_FAILURE);
    }

    char *items = realloc(command->items, capacity * sizeof(*command->items));

    if (NULL == items)
    {
        fprintf(stderr, "AllocationError: Can not reserve %zu bytes for command.\n", capacity);

        command_delete(command);
        exit(EXIT_FAILURE);
    }

    command->items = items;
    command->capacity = capacity;
}

#ifdef _WIN32
#ifndef WIN32_COMMAND_ERROR_MESSAGE_SIZE
#define WIN32_COMMAND_ERROR_MESSAGE_SIZE (4 * 1024)
#endif // WIN32_COMMAND_ERROR_MESSAGE_SIZE

/**
 * @brief Format a windows error code as a string.
 * @param error Error code to format.
 * @returns The given error code formatted as a string.
 */
static char *__command_error_message_windows(DWORD error)
{
    static _Thread_local char win32_error_message[WIN32_COMMAND_ERROR_MESSAGE_SIZE] = {0};
    DWORD error_message_size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, LANG_USER_DEFAULT, win32_error_message, WIN32_COMMAND_ERROR_MESSAGE_SIZE, NULL);
    if (error_message_size == 0)
    {
        if (GetLastError() != ERROR_MR_MID_NOT_FOUND)
        {
            if (sprintf(win32_error_message, "Could not get error message for 0x%lX", error) <= 0) return NULL;
            return win32_error_message;
        }
        else if (sprintf(win32_error_message, "Invalid windows error code (0x%lX)", error) <= 0) return NULL;
        return win32_error_message;

    }
    while (error_message_size > 1 && isspace(win32_error_message[error_message_size - 1])) { win32_error_message[--error_message_size] = '\0'; }
    return win32_error_message;
}

/**
 * @brief Obtain a checked handle from a DWORD.
 * @param handle Handle to check.
 * @param command Command to assign.
 * @returns A checked handle.
 * @exception If the given handle is invalid, a `ValueError` is printed to standard error and the programme exits.
 */
static HANDLE _check_handle(DWORD handle, command_t *command)
{
    HANDLE result = GetStdHandle(handle);
    if (INVALID_HANDLE_VALUE == result || NULL == result)
    {
        fprintf(stderr, "ValueError: Can not access handle. %s\n", __command_error_message_windows(GetLastError()));
        command_delete(command);
        exit(EXIT_FAILURE);
    }
    return result;
}

#endif // _WIN32

/**
 * @brief Run a command asynchronously.
 * @param command Command to run.
 * @returns A handle to the process of the command.
 * @returns If the command could not run, `INVALID_PROCESS` is returned.
 */
process_t command_run_async(command_t *command)
{
    if (command->size == 0) return INVALID_PROCESS;
#ifdef _WIN32
    STARTUPINFO start_info;
    ZeroMemory(&start_info, sizeof(start_info));
    start_info.cb = sizeof(STARTUPINFO);
    start_info.hStdOutput = _check_handle(STD_OUTPUT_HANDLE, command);
    start_info.hStdInput = _check_handle(STD_INPUT_HANDLE, command);
    start_info.hStdError = _check_handle(STD_ERROR_HANDLE, command);
    start_info.dwFlags |= STARTF_USESTDHANDLES;
    PROCESS_INFORMATION process_info;
    ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
    BOOL success = CreateProcess(NULL, command_items(command), NULL, NULL, TRUE, 0, NULL, NULL, &start_info, &process_info);
    if (!success) return INVALID_PROCESS;
    else if (!process_close(process_info.hThread)) return INVALID_PROCESS;
    return process_info.hProcess;
#else
#error "NotImplementedError: The linux implementation of 'command_run_async' has not been implemented yet."
#endif // _WIN32
}

/**
 * @brief Run a command asynchronously logged.
 * @param command Command to run.
 * @param logger Logger with which to log the command.
 * @returns A handle to the process of the command.
 * @returns If the command could not run, `INVALID_PROCESS` is returned.
 */
process_t command_run_async_logged(command_t *command, const logger_t *logger)
{
    size_t checkpoint = buffer_save();
    logger_log(logger, buffer_sprintf("Running command: %s.", command_data(command)), LOG_INFO);
    buffer_rewind(checkpoint);
    return command_run_async(command);
}

/**
 * @brief Run a command synchronously.
 * @param command Command to run.
 * @returns True if the command has run successfully, else false.
 */
bool command_run(command_t *command)
{
    return process_wait(command_run_async(command));
}

/**
 * @brief Run a command synchronously logged.
 * @param command Command to run.
 * @param logger Logger with which to log the command.
 * @returns True if the command has run successfully, else false.
 */
bool command_run_logged(command_t *command, const logger_t *logger)
{
    size_t checkpoint = buffer_save();
    logger_log(logger, buffer_sprintf("Running command: %s.", command_data(command)), LOG_INFO);
    buffer_rewind(checkpoint);
    return command_run(command);
}

/**
 * @brief Deallocate the command.
 * @param command Command to deallocate.
 */
void command_delete(command_t *command)
{
    if (!command->items) return;
    free(command->items);
    command->items = NULL;
    command->capacity = 0;
    command->size = 0;
}

#if defined(__cplusplus)
}
#endif

#endif // COMMAND_IMPLEMENTATION