#include <usbdrvce.h>
#include <ti/getcsc.h>
#include <ti/screen.h>

#include "lwip/init.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"

#include "lwip/altcp_tcp.h"
#include "lwip/altcp.h"
#include "lwip/dhcp.h"

#include "drivers/usb-ethernet.h"

#include "netchat-lib/netchat.h"

enum
{
    INPUT_LOWER,
    INPUT_UPPER,
    INPUT_NUMBER
};
char *chars_lower = "\0\0\0\0\0\0\0\0\0\0\"wrmh\0\0?[vqlg\0\0.zupkfc\0 ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
char *chars_upper = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
char *chars_num = "\0\0\0\0\0\0\0\0\0\0+-*/^\0\0?359)\0\0\0.258(\0\0\0000147,\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char mode_indic[] = {'a', 'A', '1'};
bool outchar_scroll_up = true;

struct netif *ethif = NULL;
err_t tcp_connect_callback(void *arg, struct altcp_pcb *tpcb, err_t err);

bool run_main = false;

ChatServer default_server = {
    .host = "change to host",
    .port = 2052,
    .online_mode = false,
    .server_password = ""
};

static void newline(void)
{
    if (outchar_scroll_up)
    {
        memmove(gfx_vram, gfx_vram + (LCD_WIDTH * 10), LCD_WIDTH * (LCD_HEIGHT - 30));
        gfx_SetColor(255);
        gfx_FillRectangle_NoClip(0, LCD_HEIGHT - 30, LCD_WIDTH, 10);
        gfx_SetTextXY(2, LCD_HEIGHT - 30);
    }
    else
        gfx_SetTextXY(2, gfx_GetTextY() + 10);
}
void outchar(char c)
{
    if (c == '\n')
    {
        newline();
    }
    else if (c < ' ' || c > '~')
    {
        return;
    }
    else
    {
        if (gfx_GetTextX() >= LCD_WIDTH - 16)
        {
            newline();
        }
        gfx_PrintChar(c);
    }
}

void ethif_status_callback_fn(struct netif *netif)
{
    if (netif->flags & NETIF_FLAG_LINK_UP)
    {
        printf("Link up\n");
    }
    else
    {
        printf("Link down\n");
    }
}

void exit_funcs(void)
{
    usb_Cleanup();
    gfx_End();
}

void message_received_callback(IncomingMessage msg)
{
    printf("[%lu] %s: %s\n", msg.timestamp, msg.sender, msg.message);
    return;
}

void handle_all_events()
{
    usb_HandleEvents();       // usb events
    sys_check_timeouts();     // lwIP timers/event callbacks=
}

int main(void)
{
    sk_key_t key = 0;
    char *ref_str = chars_lower;
    uint8_t input_mode = INPUT_LOWER;
    uint8_t string_length = 0;
    char chat_string[64] = {0};
    bool string_changed = true;

    os_ClrHome();
    gfx_Begin();
    newline();
    gfx_SetTextXY(2, LCD_HEIGHT - 30);
    printf("NETCHAT\n");
    lwip_init();

    if (usb_Init(eth_handle_usb_event, NULL, NULL, USB_DEFAULT_INIT_FLAGS))
        goto exit;

    run_main = true;

    printf("Waiting for interface...\n");
    while (1)
    {
        key = os_GetCSC();
        if (key == sk_Clear) {
            run_main = false;
        }
        if (ethif == NULL)
        {
            ethif = netif_find("en0");
            if (ethif)
            {
                printf("netif found\n");
                printf("DHCP starting...\n");
                netif_set_status_callback(ethif, ethif_status_callback_fn);
                dhcp_start(ethif);
                printf("dhcp started\n");
                break;
            }
        }
        handle_all_events();
    }

    printf("waiting for DHCP to complete...\n");
    while (!dhcp_supplied_address(ethif))
    {
        key = os_GetCSC();
        if (key == sk_Clear) {
            goto exit;
        }
        handle_all_events();
    }

    printf("Initiating connection to %s:%d\n", default_server.host, default_server.port);
    netchat_init(ethif, default_server, message_received_callback);

    printf("waiting for connection to complete...\n");
    while (!netchat_is_connected()) {
        key = os_GetCSC();
        if (key == sk_Clear) {
            goto exit;
        }
        handle_all_events();
    }
    printf("Connection established!\n");
    printf("Server online mode: %s\n", default_server.online_mode ? "enabled" : "disabled");
    run_main = true;

    printf("Press enter to login as test user\n");
    while (run_main)
    {
        key = os_GetCSC();
        if (key == sk_Clear)
        {
            run_main = false;
        }
        if (!netchat_is_logged_in())
        {
            if (key == sk_Enter)
            {
                if (netchat_login(
                    "calcuser4test",
                    "JQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcjJQGmqiXCOnUmxJucrdntpwQBygsNlNcj"
                    ) == NETCHAT_OK
                )
                {
                    printf("Login request success\n");
                    printf("Waiting for login response...\n");
                    while (!netchat_is_logged_in())
                    {
                        key = os_GetCSC();
                        if (key == sk_Clear) {
                            goto exit;
                        }
                        handle_all_events();
                    }
                }
                else
                {
                    printf("Login request failed\n");
                }
            }
        } else {
            if (key == sk_Mode || key == sk_Alpha)
            {
                input_mode++;
                if (input_mode > INPUT_NUMBER)
                    input_mode = INPUT_LOWER;
                ref_str = (input_mode == INPUT_LOWER) ? chars_lower : (input_mode == INPUT_UPPER) ? chars_upper : chars_num;
                string_changed = true;
            }
            else if (key == sk_Del)
            {
                if (string_length > 0)
                {
                    chat_string[--string_length] = 0;
                    string_changed = true;
                }
            }
            else if (key == sk_Enter)
            {
                if (string_length > 0)
                {
                    printf("Sending message: %s\n", chat_string);
                    memset(chat_string, 0, string_length);
                    string_length = 0;
                    string_changed = true;
                }
            }
            else if (ref_str[key] && (string_length < 64))
            {
                chat_string[string_length++] = ref_str[key];
                string_changed = true;
            }
            if (string_changed)
            {
                OutgoingMessage msg;
                msg.recipient = "global";
                msg.message = chat_string;
                netchat_send(&msg);
                outchar_scroll_up = false;
                gfx_SetColor(255);
                gfx_FillRectangle(0, 220, 320, 20);
                gfx_SetColor(0);
                gfx_HorizLine(0, 220, 320);
                gfx_SetTextXY(0, 221);
                printf("%s", chat_string);
                gfx_SetTextFGColor(11);
                gfx_PrintChar(mode_indic[input_mode]);
                gfx_SetTextFGColor(0);
                gfx_SetTextXY(2, LCD_HEIGHT - 30);
                string_changed = false;
                outchar_scroll_up = true;
            }
        }
        handle_all_events();
    }
    goto exit;
exit:
    exit_funcs();
    exit(0);
}
