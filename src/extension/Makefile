CC      ?= gcc
CFLAGS  ?= -std=c17 -g \
        -D_POSIX_SOURCE -D_DEFAULT_SOURCE \
        -Wall -Werror -pedantic -I.

# HOW TO COMPILE RAYLIB
# https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux
RAYLIB_INCLUDE = ../../raylib/src
RAYLIB_LIB = ../../raylib/src
RAYGUI_INCLUDE = ../../raygui/src

CFLAGS += -I$(RAYLIB_INCLUDE) -I$(RAYGUI_INCLUDE)
LDLIBS = -L$(RAYLIB_LIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

$(shell mkdir -p bin)

# SOURCE FILES
SHARED_SRCS = \
	shared/board.c

SERVER_SRCS = \
	server/main.c \
	server/game.c \
	server/network.c \
	shared/network.c

CLIENT_CORE_SRCS = \
	client/main.c \
	client/state.c \
	client/network.c \
	client/game.c \
	shared/network.c

CLIENT_GUI_SRCS = \
	client/render_raylib.c \
	client/input_raylib.c \

CLIENT_CLI_SRCS = \
	client/render_text.c \
	client/input_text.c

TEST_SRCS = \
	test/test_extension.c \
	test/test_board.c \
	shared/board.c

# OBJECT FILES
SHARED_OBJS = $(SHARED_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_CORE_OBJS = $(CLIENT_CORE_SRCS:.c=.o)
CLIENT_GUI_OBJS = $(CLIENT_GUI_SRCS:.c=.o)
CLIENT_CLI_OBJS = $(CLIENT_CLI_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

# BUILD TARGETS AND RULES
.PHONY: all clean format server client-gui client-client demo

all: bin/server bin/client-gui bin/client-cli

bin/server: $(SHARED_OBJS) $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@
server: bin/server

bin/client-gui: $(SHARED_OBJS) $(CLIENT_CORE_OBJS) $(CLIENT_GUI_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
client-gui: bin/client-gui

bin/client-cli: $(SHARED_OBJS) $(CLIENT_CORE_OBJS) $(CLIENT_CLI_OBJS)
	$(CC) $(CFLAGS) $^ -o $@
client-cli: bin/client-cli

bin/demo: main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
demo: bin/demo

bin/test: $(TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@
test: bin/test

# Compiles any missing .o file, and places next to its .c counterpart
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# FORMAT
format:
	git ls-files '*.c' '*.h' | xargs clang-format -i

# CLEAN
clean:
	-$(RM) -r bin/*
	-$(RM) $(SHARED_OBJS) $(SERVER_OBJS) $(CLIENT_CORE_OBJS) $(CLIENT_GUI_OBJS) $(CLIENT_CLI_OBJS) main.o
