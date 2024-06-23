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
    printf("%s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}

void exit_funcs(void)
{
    usb_Cleanup();
    gfx_End();
}

int main(void)
{
    sk_key_t key = 0;
    char *ref_str = chars_lower;
    os_ClrHome();
    gfx_Begin();
    newline();
    gfx_SetTextXY(2, LCD_HEIGHT - 30);
    printf("lwIP private beta test\n");
    printf("Simple TCP Text Chat\n");
    lwip_init();
    if (usb_Init(eth_handle_usb_event, NULL, NULL, USB_DEFAULT_INIT_FLAGS))
        return 1;

    if (usb_Init(eth_handle_usb_event, NULL, NULL, USB_DEFAULT_INIT_FLAGS))
        goto exit;

    run_main = true;

    do
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
                netif_set_status_callback(ethif, ethif_status_callback_fn);
                dhcp_start(ethif);
                printf("dhcp started\n");
            }
        }
        usb_HandleEvents();       // usb events
        sys_check_timeouts();     // lwIP timers/event callbacks
    } while (run_main);
exit:
    usb_Cleanup();
    exit(0);
}
