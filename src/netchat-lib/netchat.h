/*
 *--------------------------------------
 * Program Name: NETCHAT Chat system
 * Author: TKB Studios
 * License: Apache License 2.0
 * Description: Allows the user to communicate with a NETCHAT server
 *--------------------------------------
*/

#ifndef NETCHAT_H
#define NETCHAT_H

#include <stdbool.h>
#include <time.h>
#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

// NETCHAT Server information
typedef struct {
    char *host;             // domain or IP address
    int port;               // server port
    bool online_mode;       // online mode (true or false)
    char *server_password;  // server password
} ChatServer;

// Incoming message (for ex. when receiving a message)
typedef struct {
    time_t timestamp;       // message timestamp (UNIX time)
    char *recipient;        // message recipient (min. 3 chars, max. 18 chars)
    char *sender;           // message sender (min. 3 chars, max. 18 chars)
    char *message;          // message content
} IncomingMessage;

// Outgoing message (for ex. when sending a message)
typedef struct {
    char *recipient;        // message recipient (min. 3 chars, max. 18 chars)
    char *message;          // message content
} OutgoingMessage;

typedef void (*received_message_callback_t)(IncomingMessage *message);

/*
 * Initialize the chat system
 * @param server server information
 * @param received_message_callback function to call when a message is received
*/
void chat_init(struct netif *netif, ChatServer *server, received_message_callback_t received_message_callback);

/*
 * Send a message
 * @param message message to send
*/
void chat_send(OutgoingMessage *message);

/*
 * This will handle all events needed for the chat to work.
 * for ex. read from TCP and write to TCP
*/
void chat_handle_events();

/*
 * Destroy the chat system
*/
void chat_destroy();

#ifdef __cplusplus
}
#endif

#endif
