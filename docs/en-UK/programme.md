# Programme
This module provides an interface to run multiple [command](/docs/en-UK/command.md)s.

## Table of Contents
1. [Construction](#construction)
2. [Usage](#usage)
3. [Destruction](#destruction)

### Construction
There are many functions defined to construct a new `programme_t`. Defined are: `programme_init`, `programme_init_with_capacity`, `programme_init_with_logger`, and `programme_init_full`. Each of these functions will return a new `heap-allocated` `programme_t`.
### Usage
A programme will need to first have a `command_t` appended to its internal array. After the internal command array has data, the functions `programme_run` and `programme_run_async` are defined. These functions will run each of the commands within the internal array of the programme. Both of these functions have a variant with the addition of `_logged` on the end of the function name that take in a [logger](https://github.com/Polshkrev/lib/blob/main/docs/en-UK/c/logger.md) already included in the header.
### Destruction
To deallocate the programme the function `programme_delete` will need to be called.