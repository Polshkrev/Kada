# Kada
A build system in c/c++.

## Table of Contents
1. [Installation](#installation)
2. [Hello World](#hello-world)
1. [Process](/docs/en-UK/process.md)
2. [Process Array](/docs/en-UK/process_array.md)
3. [Command](/docs/en-UK/command.md)
4. [Programme](/docs/en-UK/programme.md)

## Installation
To install Kada, the recommended way is to download the entire library and add it to your project. The individual headers can also be downloaded and added to your project. The main entry points are defined in [command](/docs/en-UK/command.md) for a single command, or [programme](/docs/en-UK/programme.md) for sequential commands.

## Hello World
A "Hello World" in Kada can be defined thusly:
```c
#define COMMAND_IMPLEMENTATION
#include "command.h"

int main(void)
{
    command_t command = command_init();
    command_append(&command, "echo 'Hello World!'");
    if (!command_run(&command))
    {
        fprintf(stderr, "Can not run command: '%s'.\n", command_data(&command));
        command_delete(&command);
        return 1;
    }
    command_delete(&command);
    return 0;
}
```