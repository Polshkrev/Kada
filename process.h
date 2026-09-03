#ifndef PROCESS_H
#define PROCESS_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h> // bool

#ifdef _WIN32
#include <windef.h> // HANDLE, DWORD
#include <processthreadsapi.h> // GetCurrentProcess, WaitForSingleObject, GetExitCodeProcess, CloseHandle
#include <handleapi.h> // INVALID_HANDLE_VALUE
#include <winbase.h> // INFINITE, WAIT_FAILED
/**
 * @brief Representation of a process.
 */
typedef HANDLE process_t;
/**
 * @brief Representation of an invalid process.
 */
#define INVALID_PROCESS INVALID_HANDLE_VALUE
#else
/**
 * @brief Representation of a process.
 */
typedef int process_t;
/**
 * @brief Representation of an invalid process.
 */
#define INVALID_PROCESS (-1)
#include <sys/wait.h> // waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED
#include <unistd.h> // getpid
#endif // _WIN32

/**
 * @brief Obtain the current process.
 * @returns A `process_t` representing the current process.
 */
process_t current_process(void);

/**
 * @brief Wait on a single given process.
 * @param process Process on which to be waited.
 * @returns True if the process can be waited on, else false.
 */
bool process_wait(process_t process);

/**
 * @brief Determine if the given process is invalid.
 * @returns True if the given process is determined to be valid, else false.
 */
bool process_invalid(process_t process);

#if defined(__cplusplus)
}
#endif

#endif // PROCESS_H

#ifdef PROCESS_IMPLEMENTATION

#if defined(__cplusplus)
extern "C" {
#endif

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

#ifdef _WIN32

/**
 * @brief Wait on a single process implementation.
 * @param process Process on which to be waited.
 * @returns True if the process can be waited on, else false.
 */
bool _process_wait(process_t process)
{
    DWORD result = WaitForSingleObject(process, INFINITE);
    if (result == WAIT_FAILED) return false;
    DWORD exit_status;
    if (!GetExitCodeProcess(process, &exit_status)) return false;
    else if (exit_status != 0) return false;
    return CloseHandle(process);
}

#else

/**
 * @brief Wait on a single process implementation.
 * @param process Process on which to be waited.
 * @returns True if the process can be waited on, else false.
 */
bool _process_wait(process_t process)
{
    for (;;) 
    {
        int status = 0;
        if (waitpid(process, &status, 0) < 0) return false;
        else if (WIFEXITED(status)) return WEXITSTATUS(status) == 0;
        else if (WIFSIGNALED(status)) return false;
    }
    return true;
}

#endif // _WIN32

/**
 * @brief Wait on a single given process.
 * @param process Process on which to be waited.
 * @returns True if the process can be waited on, else false.
 */
bool process_wait(process_t process)
{
    if (process_invalid(process)) return false;
    return _process_wait(process);
}

/**
 * @brief Determine if the given process is invalid.
 * @returns True if the given process is determined to be valid, else false.
 */
bool process_invalid(process_t process)
{
    return process == INVALID_PROCESS;
}

#if defined(__cplusplus)
}
#endif

#endif // PROCESS_IMPLEMENTATION