#ifndef COMMAND_H
#define COMMAND_H

#if defined(__cplusplus)
extern "C" {
#endif

#define PROCESSES_IMPLEMENTATION
#include "processes.h"

#define LOGGER_IMPLEMENTATION
#include "lib/c/logger.h"

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
command_t *command_init(void);

/**
 * @brief Construct a new command of a given capacity.
 * @param capacity Initial capacity of the command.
 * @returns A new command with a given capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t *command_init_with_capacity(size_t capacity);

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
 */
void command_appendf(command_t *command, const char *format, ...);

/**
 * @brief Access the command at a given index.
 * @param command Command to access.
 * @param index Index at which to access the command.
 * @returns A pointer to the character stored within the command at a given index.
 * @exception If the index is greater than the size of the command, an `OutOfRangeError` is is printed to standard error and the programme exits.
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
 * @returns True if the command has ran successfully, else false.
 */
process_t command_run_async(command_t *command);

/**
 * @brief Run a command asynchronously logged.
 * @param command Command to run.
 * @param logger Logger from which to read.
 * @returns True if the command has ran successfully, else false.
 */
process_t command_run_async_logged(command_t *command, const logger_t *logger);

/**
 * @brief Run a command synchronously.
 * @param command Command to run.
 * @returns True if the command has ran successfully, else false.
 */
bool command_run(command_t *command);

/**
 * @brief Run a command synchronously logged.
 * @param command Command to run.
 * @param logger Logger from which to read.
 * @returns True if the command has ran successfully, else false.
 */
bool command_run_logged(command_t *command, const logger_t *logger);

/**
 * @brief Resize the command by a factor of two.
 * @param command Command to resize.
 * @exception If the command can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void command_resize(command_t *command);

/**
 * @brief Resize the command by a given scaler value.
 * @param command Command to resize.
 * @param scaler Scaler value by which to resize the command.
 * @exception If the command can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void command_resize_by(command_t *command, size_t scaler);

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

#include <stdarg.h> // va_list, va_start, va_end

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // DWORD
#ifndef WIN32_ERROR_MESSAGE_SIZE
#define WIN32_ERROR_MESSAGE_SIZE (4 * 1024)
#endif // WIN32_ERROR_MESSAGE_SIZE
#endif // _WIN32

#define BUFFER_IMPLEMENTATION
#include "lib/c/collections/buffer.h"

#define COMMAND_INITIAL_CAPACTIY 256

/**
 * @brief Construct a new command with a fixed capacity.
 * @returns A new command with a fixed capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t *command_init(void)
{
    return command_init_with_capacity(COMMAND_INITIAL_CAPACTIY);
}

/**
 * @brief Construct a new command of a given capacity.
 * @param capacity Initial capacity of the command.
 * @returns A new command with a given capacity.
 * @exception If the command can not be allocated on the heap, an `AllocationError` is printed to standard error and the programme exits.
 */
command_t *command_init_with_capacity(size_t capacity)
{
    command_t *command = (command_t *)malloc(sizeof(command_t));
    if (NULL == command)
    {
        fprintf(stderr, "AllocationError: Can not allocate enough memory for a new command.\n");
        exit(1);
    }
    command->size = 0;
    command->capacity = capacity;
    command->items = (char *)malloc(sizeof(char) * capacity);
    if (NULL == command->items)
    {
        fprintf(stderr, "AllocationError: Can not allocate enough memory for a new item array.\n");
        if (command) free(command);
        exit(1);
    }
    return command;
}

/**
 * @brief Internal charactrer appendation function.
 * @param command Command to which to append.
 * @param item Item to append to the command.
 * @exception If the command can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void _command_append(command_t *command, char item)
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
 */
void command_appendf(command_t *command, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int n = vsnprintf(NULL, 0, format, arguments);
    va_end(arguments);
    if (n < 0)
    {
        fprintf(stderr, "AllocationError: 'vsnprintf' has failed.\n");
        command_delete(command);
        exit(1);
    }
    size_t checkpoint = buffer_save();
    char *result = buffer_allocate(n + 1);
    if (NULL == result)
    {
        fprintf(stderr, "AllocationError: Can not allocate enough memory to append a formatted string.\n");
        command_delete(command);
        exit(1);
    }
    va_start(arguments, format);
    vsnprintf(result, n + 1, format, arguments);
    va_end(arguments);
    command_append(command, result);
    buffer_rewind(checkpoint);
}

/**
 * @brief Access the command at a given index.
 * @param command Command to access.
 * @param index Index at which to access the command.
 * @returns A pointer to the character stored within the command at a given index.
 * @exception If the index is greater than the size of the command, an `OutOfRangeError` is is printed to standard error and the programme exits.
 */
char *command_at(command_t *command, size_t index)
{
    if (index >= command->size)
    {
        fprintf(stderr, "IndexError: Can not access array of size %zu at index %zu.\n", command->size, index);
        command_delete(command);
        exit(1);
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
    _command_append(command, '\0');
    return command_items(command);
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
 * @exception If the command can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void command_resize(command_t *command)
{
    command_resize_by(command, 2);
}

/**
 * @brief Resize the command by a given scaler value.
 * @param command Command to resize.
 * @param scaler Scaler value by which to resize the command.
 * @exception If the command can not be reallocated, an `AllocationError` is is printed to standard error and the programme exits.
 */
void command_resize_by(command_t *command, size_t scaler)
{
    command->capacity *= scaler;
    command->items = (char *)realloc(command->items, command->capacity);
    if (NULL == command->items)
    {
        fprintf(stderr, "AllocationError: Can not resize array.\n");
        command_delete(command);
        exit(1);
    }
}

#ifdef _WIN32

/**
 * @brief Format a windows error code as a string.
 * @param error Error code to format.
 * @returns The given error code formatted as a string.
 */
char *win32_error_message(DWORD error)
{
    static char win32_error_message[WIN32_ERROR_MESSAGE_SIZE] = {0};
    DWORD error_message_size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, LANG_USER_DEFAULT, win32_error_message, WIN32_ERROR_MESSAGE_SIZE, NULL);
    if (error_message_size == 0)
    {
        if (GetLastError() != ERROR_MR_MID_NOT_FOUND)
        {
            if (sprintf(win32_error_message, "Could not get error message for 0x%lX", error) > 0)
            {
                return (char *)&win32_error_message;
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            if (sprintf(win32_error_message, "Invalid windows error code (0x%lX)", error) > 0)
            {
                return (char *)&win32_error_message;
            }
            else
            {
                return NULL;
            }
        }
    }
    while (error_message_size > 1 && isspace(win32_error_message[error_message_size - 1]))
    {
        win32_error_message[--error_message_size] = '\0';
    }
    return win32_error_message;
}

/**
 * @brief Obtain a checked handle from a DWORD.
 * @param handle Handle to check.
 * @param command Command to assign.
 * @returns A checked handle.
 * @exception If the given handle is invalid, a `ValueError` is printed to standard error and the programme exits.
 */
HANDLE _check_handle(DWORD handle, command_t *command)
{
    HANDLE result = GetStdHandle(handle);
    if (INVALID_HANDLE_VALUE == result || NULL == result)
    {
        fprintf(stderr, "ValueError: Can not access handle. %s\n", win32_error_message(GetLastError()));
        command_delete(command);
        exit(1);
    }
    return result;
}

#endif // _WIN32

/**
 * @brief Run a command asynchronously.
 * @param command Command to run.
 * @returns True if the command has ran successfully, else false.
 */
process_t command_run_async(command_t *command)
{
    if (command->size < 1) return INVALID_PROCESS;
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
    _command_append(command, '\0');
    BOOL success = CreateProcessA(NULL, command_items(command), NULL, NULL, TRUE, 0, NULL, NULL, &start_info, &process_info);
    if (!success) return INVALID_PROCESS;
    CloseHandle(process_info.hThread);
    return process_info.hProcess;
#else
#error "NotImplementedError: The linux implementation of 'command_run_async' has not been implemented yet."
#endif // _WIN32
}

/**
 * @brief Run a command asynchronously logged.
 * @param command Command to run.
 * @param logger Logger from which to read.
 * @returns True if the command has ran successfully, else false.
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
 * @returns True if the command has ran successfully, else false.
 */
bool command_run(command_t *command)
{
    return process_wait(command_run_async(command));
}

/**
 * @brief Run a command synchronously logged.
 * @param command Command to run.
 * @param logger Logger from which to read.
 * @returns True if the command has ran successfully, else false.
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
    if (!command) return;
    free(command);
    command = NULL;
}

#if defined(__cplusplus)
}
#endif

#endif // COMMAND_IMPLEMENTATION