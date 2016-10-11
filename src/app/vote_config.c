#include "app/vote_config.h"

#include <stdlib.h>
#include <command.h>

#include "system/util.h"
#include "app/feedback.h"
#include "app/storage.h"

uint8_t tempVal __attribute__((aligned(4)));

static void update_vote_config_field(uint8_t newVal, uint16_t recordKey) {
  tempVal = newVal;
  if(update_data_in_storage(&tempVal, sizeof(tempVal), VOTE_CONFIG_STORAGE_FILE_ID, recordKey) != SUCCESS) {
    display_failure_feedback();
  } else {
    display_successful_update_feedback();
  }
}

static uint8_t get_stored_vote_config(uint16_t recordKey) {
  uint8_t storedVoteConfig = INVALID_VOTE_CONFIG;
  get_data_from_storage(VOTE_CONFIG_STORAGE_FILE_ID, recordKey, &storedVoteConfig, sizeof(storedVoteConfig));
  return storedVoteConfig;
}

static void clear_vote_config(char **parsedCommandArray, uint8_t numCommands) {
  SYS_UNUSED_PARAMETER(parsedCommandArray);
  SYS_UNUSED_PARAMETER(numCommands);
  delete_file_from_storage(VOTE_CONFIG_STORAGE_FILE_ID);
}

bool vote_config_initialize() {
  mesh_add_terminal_command("group", "Update group for this node", set_vote_config_group);
  mesh_add_terminal_command("value", "Update value for this node", set_vote_config_value);
  mesh_add_terminal_command("clrvc", "Clear vote configuration", clear_vote_config);

  return true;
}

void update_voting_group(uint8_t newGroup) {
  update_vote_config_field(newGroup, VOTE_CONFIG_GROUP_STORAGE_RECORD_KEY);
}

void update_voting_value(uint8_t newValue) {
  update_vote_config_field(newValue, VOTE_CONFIG_VALUE_STORAGE_RECORD_KEY);
}

bool get_vote_configuration(voteConfiguration *result) {
  result->group = get_stored_vote_config(VOTE_CONFIG_GROUP_STORAGE_RECORD_KEY);
  result->value = get_stored_vote_config(VOTE_CONFIG_VALUE_STORAGE_RECORD_KEY);
  return (result->group != INVALID_VOTE_CONFIG && result->value != INVALID_VOTE_CONFIG);
}

bool vote_config_is_set() {
#ifdef IS_PROD_BOARD
  voteConfiguration tempVoteConfig;
  return get_vote_configuration(&tempVoteConfig);
#else
  return true;
#endif
}

void set_vote_config_group(char **parsedCommandArray, uint8_t numCommands) {
  SYS_UNUSED_PARAMETER(numCommands);
  update_voting_group((uint8_t) atoi(parsedCommandArray[1]));
}

void set_vote_config_value(char **parsedCommandArray, uint8_t numCommands) {
  SYS_UNUSED_PARAMETER(numCommands);
  update_voting_value((uint8_t) atoi(parsedCommandArray[1]));
}
