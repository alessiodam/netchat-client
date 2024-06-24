# ----------------------------
# Makefile Options
# ----------------------------

NAME = NETCHAT
ICON = icon.png
DESCRIPTION = Official NETCHAT Client

APP_NAME = NETCHAT
APP_VERSION = 0

CFLAGS = -Wall -Wextra -Oz -I src/include -I src/apps/altcp_tls/mbedtls/include
CXXFLAGS = -Wall -Wextra -Oz -Isrc/include
OUTPUT_MAP = YES

# ----------------------------

include app_tools/makefile
