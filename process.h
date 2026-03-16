#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h> // bool

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // HANDLE, DWORD, INFINITE, WAIT_FAILED, WaitForSingleObject, GetExitCodeProcess, CloseHandle
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
#endif // _WIN32

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

#endif // PROCESS_H

#ifdef PROCESS_IMPLEMENTATION

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

#endif // PROCESS_IMPLEMENTATION