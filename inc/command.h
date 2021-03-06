#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*mesh_command_handler)(char **parsedCommandArray, uint8_t numCommands);

bool command_initialize(void);
void command_execute(char** parsedCommandArray, uint8_t numberOfItems);
void mesh_add_terminal_command(char* commandName, char* commandDescription, mesh_command_handler callback);
