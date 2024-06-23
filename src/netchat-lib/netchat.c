/*
 *--------------------------------------
 * Program Name: NETCHAT Chat system
 * Author: TKB Studios
 * License: Apache License 2.0
 * Description: Allows the user to communicate with a NETCHAT server
 *--------------------------------------
*/

#include "netchat.h"

bool tcp_connected = false;

void chat_init(struct netif *netif, ChatServer *server, received_message_callback_t received_message_callback) {
    printf("[netchat-lib] chat_init\n");
    tcp_connected = false;
}

void chat_send(OutgoingMessage *message) {
    printf("[netchat-lib] chat_send\n");
    if (!tcp_connected) {
        return;
    }
}

void chat_handle_events() {
    printf("[netchat-lib] chat_handle_events\n");
}

void chat_destroy() {
    printf("[netchat-lib] chat_destroy\n");
    tcp_connected = false;
}