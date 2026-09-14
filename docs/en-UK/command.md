# Command
This module provides an interface to run a single command.

## Table of Contents
1. [Construction](#construction)
2. [Usage](#usage)
3. [Destruction](#destruction)

### Construction
A `command_t` can be initialized using the functions `command_init` and `command_init_with_capacity`. These functions will return a new `heap-allocated` `command_t`.
### Usage
A command will need to appended to using either one of the functions `command_append` and `command_appendf`. `command_append` will simply append a null-terminated string, while `command_appendf` is similar to `sprintf` in appending to the command. The command will then need to be ran. The functions defined for running a command are `command_run` and `command_run_async`. As the names suggest, these functions will run a single command synchronously or asynchronously. Both of these functions have a variant with the addition of `_logged` on the end of the function name that take in a [logger](https://github.com/Polshkrev/lib/blob/main/docs/en-UK/c/logger.md) already included in the header.
### Destruction
To deallocate the command the function `command_delete` will need to be called.