#ifndef PROCESS_H
#define PROCESS_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h> // bool
#include <stdio.h> // fprintf, FILE
#include <stdlib.h> // NULL, EXIT_SUCCESS

#ifdef _WIN32
#include <windef.h> // HANDLE, DWORD
/**
 * @brief Representation of a process.
 */
typedef HANDLE process_t;
/**
 * @brief Identifier used to inspect a process.
 */
typedef DWORD process_identifier_t;
/**
 * @brief Representation of an invalid process.
 */
#define INVALID_PROCESS NULL
/**
 * @brief Representation of an invalid process identifier.
 */
#define INVALID_PROCESS_IDENTIFIER 0
#else
#include <sys/types.h> // pid_t
/**
 * @brief Representation of a process.
 */
typedef pid_t process_t;
/**
 * @brief Identifier used to inspect a process.
 */
typedef pid_t process_identifier_t;
/**
 * @brief Representation of an invalid process.
 */
#define INVALID_PROCESS ((process_t)-1)
/**
 * @brief Representation of an invalid process identifier.
 */
#define INVALID_PROCESS_IDENTIFIER ((process_identifier_t)-1)
#endif // _WIN32

/**
 * @brief Obtain the current process.
 * @returns A `process_t` representing the current process.
 */
process_t current_process(void);

/**
 * @brief Obtain the identifier for a given process.
 * @param process Process to identify.
 * @returns The process identifier of the given process.
 */
process_identifier_t process_identifier(process_t process);

/**
 * @brief Wait for a given process to terminate.
 * @param process Process on which to wait.
 * @returns True if the process terminated successfully, otherwise false.
 */
bool process_wait(process_t process);

/**
 * @brief Determine if the given process is invalid.
 * @param process Process to evaluate.
 * @returns True if the given process is determined to be invalid, else false.
 */
bool process_invalid(process_t process);

/**
 * @brief Print a given process into a given stream.
 * @param stream Stream to which to print the given process.
 * @param process Process to print.
 */
bool process_print(FILE *stream, process_t process);

/**
 * @brief Close a given process.
 * @param process Process to close.
 * @returns True if the process was closed, else false.
 */
bool process_close(process_t process);

#if defined(__cplusplus)
}
#endif

#endif // PROCESS_H

#ifdef PROCESS_IMPLEMENTATION

#if defined(__cplusplus)
extern "C" {
#endif

#include <errno.h> // errno, EINTR

#ifdef _WIN32
    #include <processthreadsapi.h> // GetCurrentProcess, WaitForSingleObject, GetExitCodeProcess, CloseHandle
    #include <winbase.h> // INFINITE, WAIT_FAILED
#else
    #include <sys/wait.h> // waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED
    #include <unistd.h> // getpid
#endif // _WIN32

/**
 * @brief Obtain the current process.
 * @returns A `process_t` representing the current process.
 */
process_t current_process(void)
{
#ifdef _WIN32
    return GetCurrentProcess();
#else
    return getpid();
#endif // _WIN32
}

/**
 * @brief Obtain the identifier for a given process.
 * @param process Process to identify.
 * @returns The process identifier of the given process.
 */
process_identifier_t process_identifier(process_t process)
{
    if (process_invalid(process)) return INVALID_PROCESS_IDENTIFIER;
#ifdef _WIN32
    return GetProcessId(process);
#else
    return process;
#endif // _WIN32
}

#ifdef _WIN32

/**
 * @brief Wait on a single process implementation.
 * @param process Process on which to be waited.
 * @returns True if the process can be waited on, else false.
 */
static bool _process_wait(process_t process)
{
    DWORD result = WaitForSingleObject(process, INFINITE);
    if (result == WAIT_FAILED) return false;
    DWORD exit_status;
    if (!GetExitCodeProcess(process, &exit_status)) return false;
    return exit_status == EXIT_SUCCESS;
}

#else

/**
 * @brief Wait on a single process implementation.
 * @param process Process on which to be waited.
 * @returns True if the process can be waited on, else false.
 */
static bool _process_wait(process_t process)
{
    for (;;)
    {
        int status = 0;
        if (waitpid(process, &status, 0) < 0) return false;
        else if (errno == EINTR) return false;
        else if (WIFSIGNALED(status)) return false;
        else if (WIFEXITED(status)) return WEXITSTATUS(status) == EXIT_SUCCESS;
    }
}

#endif // _WIN32

/**
 * @brief Wait for a process to terminate.
 * @param process Process on which to wait.
 * @returns True if the process terminated successfully, otherwise false.
 */
bool process_wait(process_t process)
{
    if (process_invalid(process)) return false;
    return _process_wait(process);
}

/**
 * @brief Determine if the given process is invalid.
 * @param process Process to evaluate.
 * @returns True if the given process is determined to be invalid, else false.
 */
bool process_invalid(process_t process)
{
    return process == INVALID_PROCESS;
}

/**
 * @brief Print a given process into a given stream.
 * @param stream Stream to which to print the given process.
 * @param process Process to print.
 */
bool process_print(FILE *stream, process_t process)
{
    if (INVALID_PROCESS == process) return false;
    process_identifier_t identifier = process_identifier(process);
#ifdef _WIN32
    fprintf(stream, "%lu", identifier);
#else
    fprintf(stream, "%d", identifier);
#endif // _WIN32
    return true;
}

/**
 * @brief Close a given process.
 * @param process Process to close.
 * @returns True if the process was closed, else false.
 */
bool process_close(process_t process)
{
    if (process_invalid(process)) return false;
#ifdef _WIN32
    return CloseHandle(process);
#else
    (void) process;
    return true;
#endif // _WIN32
}

#if defined(__cplusplus)
}
#endif

#endif // PROCESS_IMPLEMENTATION