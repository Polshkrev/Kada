# Process Array
As the name suggests, this module defines the interface for a classical dynamic array of [processes](/docs/en-UK/process.md).

## Table of Contents
1. [Construction](#construction)
2. [Usage](#usage)
3. [Destruction](#destruction)

### Construction
The main construction of a process array is when calling a [programme](/docs/en-UK/programme.md#usage) asynchronously. Directly, there are two functions defined: `process_array_init` and `process_array_init_with_capacity`. Each of these functions return a new heap-allocated `process_array_t`.
### Usage
To use the `process_array_t`, you will need to append [process_t](/docs/en-UK/process.md)-s to the array with the function `process_array_append`. After the process array contains data, the function `process_array_wait` can be called. This function will execute each of the processes.
### Destruction
To deallocate the process array the function `process_array_delete` will need to be called.