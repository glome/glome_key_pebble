#include <pebble.h>
#include <comm.h>
#include "glome/ui.h"

//
glome settings;

/**
 * Send a soft account to the phone
 */
void send_softaccount(const char *softaccount) {
	DictionaryIterator *iter;

	app_message_outbox_begin(&iter);
	dict_write_cstring(iter, SOFTACCOUNT_KEY, softaccount);

	dict_write_end(iter);
  app_message_outbox_send();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent SOFTACCOUNT_KEY to phone");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", softaccount);
}

/**
 * Send a status to PebbleJS
 */
void send_status(uint8_t status){
	DictionaryIterator *iter;

	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, status);
	dict_write_end(iter);
  app_message_outbox_send();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent STATUS_KEY to phone: %d", status);
}

/**
 * Send a command to the phone
 */
void send_phone_command(uint8_t command){
	DictionaryIterator *iter;

	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, COMMAND_KEY, command);
	dict_write_end(iter);
  app_message_outbox_send();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent COMMAND_KEY to phone: %d", command);
}

/**
 *
 */
const char *load_softaccount() {
  // Get the soft account from persistent storage
  int result = 1;
  static char buffer[254];
  memset(buffer, '\0', sizeof(buffer));
  result = persist_read_string(GLOME_SOFTACCOUNT, buffer, sizeof(buffer));
  if (result > 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Soft account loaded from the watch: %d", result);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", buffer);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No soft account to load from the watch.");
  }

  return buffer;
}

/**
 * Saves the received data as soft account on the watch
 *
 * @param char* soft account to be saved
 * @retval bool Indicate success
 */
bool store_softaccount(char* data) {
  bool ret = false;
  // sanity check of the received data
  if (sizeof(data) > 0) {
    // save to persistent storage
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Soft account to be stored:");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", data);
    int result = persist_write_string(GLOME_SOFTACCOUNT, data);
    if (result > 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Soft account successfully stored on the watch in %d bytes.", result);
      ret = true;
    } else {
      APP_LOG(APP_LOG_LEVEL_WARNING, "Failed to store soft account.");
    }
  }

  return ret;
}

/**
 *
 */
char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

/**
 * Called when a message is received from PebbleKitJS
 */
void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple = NULL;
  tuple = malloc(sizeof(Tuple));

	tuple = dict_find(received, SOFTACCOUNT_KEY);
	if (tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received SOFTACCOUNT_KEY from phone in %d bytes", tuple->length);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", tuple->value->cstring);

    // do we need a new softaccount?
    char *res = (char*) load_softaccount();
    int len = strlen(res);
    if (len == 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Seems there is no soft account on the watch.");
      store_softaccount(tuple->value->cstring);
    } else
    {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Soft account on the watch is %d bytes:", len);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", res);
    }
    return;
	}

	tuple = dict_find(received, CODE_KEY);
	if (tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received CODE_KEY from phone: %s", tuple->value->cstring);
    // check if we are at the right window
    // display the QR
    update_qr(tuple->value->cstring);
	}

	tuple = dict_find(received, STATUS_KEY);
	if (tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received STATUS_KEY from phone: %d", (int)tuple->value->uint8);
    if ((int)tuple->value->uint8 == 1) {
      // send back something to the phone
      // lame: add 10 to the received value
      send_status(11);
    }
	}

	tuple = dict_find(received, COMMAND_KEY);
	if (tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received COMMAND_KEY from phone: %d", (int)tuple->value->uint8);
    if ((int)tuple->value->uint8 == WATCH_LOAD_SOFTACCOUNT) {
      // send back something to the phone
      char *res = (char*) load_softaccount();
      // lame check of soft account
      if (strlen(res) > 20) {
        settings.softaccount = res;
        send_softaccount(settings.softaccount);
        // todo: here we need to wait a few nanosec before
        // send_phone_command(API_CREATE_CODE);
      } else {
        send_phone_command(API_CREATE_SOFTACCOUNT);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "No soft account found; requested one from the phone.");
      }
    }
	}
}

/**
 *
 */
void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to receive message");
}

/**
 *
 */
void out_sent_handler(DictionaryIterator *iterator, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Message sent");
}

/**
 *
 */
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed sending message, reason: %s", translate_error(reason));
}

char *bool2str(bool var) {
  return var ? "true" : "false";
}
