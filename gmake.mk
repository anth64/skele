ifeq ($(OS),Windows_NT)
	CLOCK_SRC := src/platform/clock/win32.c
	INPUT_SRC := src/platform/client/input/sdl.c
else
	CLOCK_SRC := src/platform/clock/posix.c
	INPUT_SRC := src/platform/client/input/sdl.c
endif

include config.mk

ENGINE_SRCS      := src/skele.c $(CLOCK_SRC)
SOFT_PLAT_SRCS   := src/platform/client/video/sdl.c $(INPUT_SRC)
GL_PLAT_SRCS     := src/platform/client/video/gl.c $(INPUT_SRC)
SOFT_CLIENT_SRCS := example/client/main.c
GL_CLIENT_SRCS   := example/gl_client/main.c
SERVER_EXAMPLE   := example/server/main.c

LIBSKELE_SRCS       := $(ENGINE_SRCS) $(SOFT_PLAT_SRCS)
LIBGLSKELE_SRCS     := $(ENGINE_SRCS) $(GL_PLAT_SRCS)
LIBSKELE_SRV_SRCS   := $(ENGINE_SRCS)

ifeq ($(OS),Windows_NT)
	SHELL                := cmd.exe
	LIBSKELE             := $(BIN_DIR)/debug/libskele.a
	LIBGLSKELE           := $(BIN_DIR)/debug/libglskele.a
	LIBSKELE_SRV         := $(BIN_DIR)/debug/libskeleserver.a
	SKELE_BIN            := $(BIN_DIR)/debug/skele_client.exe
	GLSKELE_BIN          := $(BIN_DIR)/debug/glskele_client.exe
	SERVER_BIN           := $(BIN_DIR)/debug/skele_server.exe
	LIBSKELE_R           := $(BIN_DIR)/release/libskele.a
	LIBGLSKELE_R         := $(BIN_DIR)/release/libglskele.a
	LIBSKELE_SRV_R       := $(BIN_DIR)/release/libskeleserver.a
	SKELE_BIN_R          := $(BIN_DIR)/release/skele_client.exe
	GLSKELE_BIN_R        := $(BIN_DIR)/release/glskele_client.exe
	SERVER_BIN_R         := $(BIN_DIR)/release/skele_server.exe
	LDFLAGS_PLAT         :=
	CFLAGS_PLAT          :=
	MKDIR = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
	RMDIR = if exist $(subst /,\,$(1)) rd /s /q $(subst /,\,$(1))
else
	LIBSKELE             := $(BIN_DIR)/debug/libskele.a
	LIBGLSKELE           := $(BIN_DIR)/debug/libglskele.a
	LIBSKELE_SRV         := $(BIN_DIR)/debug/libskeleserver.a
	SKELE_BIN            := $(BIN_DIR)/debug/skele_client
	GLSKELE_BIN          := $(BIN_DIR)/debug/glskele_client
	SERVER_BIN           := $(BIN_DIR)/debug/skele_server
	LIBSKELE_R           := $(BIN_DIR)/release/libskele.a
	LIBGLSKELE_R         := $(BIN_DIR)/release/libglskele.a
	LIBSKELE_SRV_R       := $(BIN_DIR)/release/libskeleserver.a
	SKELE_BIN_R          := $(BIN_DIR)/release/skele_client
	GLSKELE_BIN_R        := $(BIN_DIR)/release/glskele_client
	SERVER_BIN_R         := $(BIN_DIR)/release/skele_server
	LDFLAGS_PLAT         :=
	CFLAGS_PLAT          :=
	MKDIR = mkdir -p $(1)
	RMDIR = rm -rf $(1)
endif

ifeq ($(OS),Windows_NT)
	GL_LIB       := -lopengl32
	LDFLAGS_PLAT :=
	CFLAGS_PLAT  :=
else ifeq ($(shell uname),FreeBSD)
	GL_LIB       := -lGL
	LDFLAGS_PLAT := -L/usr/local/lib
	CFLAGS_PLAT  := -I/usr/local/include
else
	GL_LIB       := -lGL
	LDFLAGS_PLAT :=
	CFLAGS_PLAT  :=
endif
LINK_STK        := -Wl,-Bstatic -lstk -Wl,-Bdynamic
RELEASE_LDFLAGS := -s
CFLAGS_BASE     := -Wall -Wpedantic -I$(INC_DIR) -std=c99 $(CFLAGS_PLAT) $(DEBUG_FLAGS) \
                   -DGL_MAJOR=$(GL_MAJOR) -DGL_MINOR=$(GL_MINOR)

LIBSKELE_DEBUG_OBJS      := $(LIBSKELE_SRCS:%.c=obj/debug/soft/%.o)
LIBGLSKELE_DEBUG_OBJS    := $(LIBGLSKELE_SRCS:%.c=obj/debug/gl/%.o)
LIBSKELE_SRV_DEBUG_OBJS  := $(LIBSKELE_SRV_SRCS:%.c=obj/debug/server/%.o)
SOFT_CLIENT_DEBUG_OBJS   := $(SOFT_CLIENT_SRCS:%.c=obj/debug/soft/%.o)
GL_CLIENT_DEBUG_OBJS     := $(GL_CLIENT_SRCS:%.c=obj/debug/gl/%.o)
SERVER_DEBUG_OBJS        := $(SERVER_EXAMPLE:%.c=obj/debug/server/%.o)

LIBSKELE_RELEASE_OBJS      := $(LIBSKELE_SRCS:%.c=obj/release/soft/%.o)
LIBGLSKELE_RELEASE_OBJS    := $(LIBGLSKELE_SRCS:%.c=obj/release/gl/%.o)
LIBSKELE_SRV_RELEASE_OBJS  := $(LIBSKELE_SRV_SRCS:%.c=obj/release/server/%.o)
SOFT_CLIENT_RELEASE_OBJS   := $(SOFT_CLIENT_SRCS:%.c=obj/release/soft/%.o)
GL_CLIENT_RELEASE_OBJS     := $(GL_CLIENT_SRCS:%.c=obj/release/gl/%.o)
SERVER_RELEASE_OBJS        := $(SERVER_EXAMPLE:%.c=obj/release/server/%.o)

PREFIX ?= /usr
MODE   ?= debug

.PHONY: all debug release skele glskele server libs \
        install uninstall clean run run_gl

all: debug

debug: \
	$(LIBSKELE) \
	$(LIBGLSKELE) \
	$(LIBSKELE_SRV) \
	$(SKELE_BIN) \
	$(GLSKELE_BIN) \
	$(SERVER_BIN)

release: \
	$(LIBSKELE_R) \
	$(LIBGLSKELE_R) \
	$(LIBSKELE_SRV_R) \
	$(SKELE_BIN_R) \
	$(GLSKELE_BIN_R) \
	$(SERVER_BIN_R)

libs:    $(LIBSKELE) $(LIBGLSKELE) $(LIBSKELE_SRV)
skele:   $(LIBSKELE) $(SKELE_BIN)
glskele: $(LIBGLSKELE) $(GLSKELE_BIN)
server:  $(LIBSKELE_SRV) $(SERVER_BIN)

run: skele
	$(SKELE_BIN)

run_gl: glskele
	$(GLSKELE_BIN)

$(LIBSKELE): $(LIBSKELE_DEBUG_OBJS)
	@$(call MKDIR,$(@D))
	$(AR) rcs $@ $^

$(LIBGLSKELE): $(LIBGLSKELE_DEBUG_OBJS)
	@$(call MKDIR,$(@D))
	$(AR) rcs $@ $^

$(LIBSKELE_SRV): $(LIBSKELE_SRV_DEBUG_OBJS)
	@$(call MKDIR,$(@D))
	$(AR) rcs $@ $^

$(LIBSKELE_R): $(LIBSKELE_RELEASE_OBJS)
	@$(call MKDIR,$(@D))
	$(AR) rcs $@ $^

$(LIBGLSKELE_R): $(LIBGLSKELE_RELEASE_OBJS)
	@$(call MKDIR,$(@D))
	$(AR) rcs $@ $^

$(LIBSKELE_SRV_R): $(LIBSKELE_SRV_RELEASE_OBJS)
	@$(call MKDIR,$(@D))
	$(AR) rcs $@ $^

$(SKELE_BIN): $(SOFT_CLIENT_DEBUG_OBJS) $(LIBSKELE)
	@$(call MKDIR,$(@D))
	$(CC) -o $@ $(SOFT_CLIENT_DEBUG_OBJS) -L$(BIN_DIR)/debug -lskele $(LINK_STK) -lSDL3 $(LDFLAGS_PLAT)

$(GLSKELE_BIN): $(GL_CLIENT_DEBUG_OBJS) $(LIBGLSKELE)
	@$(call MKDIR,$(@D))
	$(CC) -o $@ $(GL_CLIENT_DEBUG_OBJS) -L$(BIN_DIR)/debug -lglskele $(LINK_STK) -lSDL3 $(GL_LIB) $(LDFLAGS_PLAT)

$(SERVER_BIN): $(SERVER_DEBUG_OBJS) $(LIBSKELE_SRV)
	@$(call MKDIR,$(@D))
	$(CC) -o $@ $(SERVER_DEBUG_OBJS) -L$(BIN_DIR)/debug -lskeleserver $(LINK_STK) $(LDFLAGS_PLAT)

$(SKELE_BIN_R): $(SOFT_CLIENT_RELEASE_OBJS) $(LIBSKELE_R)
	@$(call MKDIR,$(@D))
	$(CC) $(RELEASE_LDFLAGS) -o $@ $(SOFT_CLIENT_RELEASE_OBJS) -L$(BIN_DIR)/release -lskele $(LINK_STK) -lSDL3 $(LDFLAGS_PLAT)

$(GLSKELE_BIN_R): $(GL_CLIENT_RELEASE_OBJS) $(LIBGLSKELE_R)
	@$(call MKDIR,$(@D))
	$(CC) $(RELEASE_LDFLAGS) -o $@ $(GL_CLIENT_RELEASE_OBJS) -L$(BIN_DIR)/release -lglskele $(LINK_STK) -lSDL3 $(GL_LIB) $(LDFLAGS_PLAT)

$(SERVER_BIN_R): $(SERVER_RELEASE_OBJS) $(LIBSKELE_SRV_R)
	@$(call MKDIR,$(@D))
	$(CC) $(RELEASE_LDFLAGS) -o $@ $(SERVER_RELEASE_OBJS) -L$(BIN_DIR)/release -lskeleserver $(LINK_STK) $(LDFLAGS_PLAT)

obj/debug/soft/%.o: %.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS_BASE) -g -O0 -MMD -MP -c $< -o $@

obj/debug/gl/%.o: %.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS_BASE) -g -O0 -MMD -MP -c $< -o $@

obj/debug/server/%.o: %.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS_BASE) -g -O0 -MMD -MP -c $< -o $@

obj/release/soft/%.o: %.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS_BASE) -O2 -MMD -MP -c $< -o $@

obj/release/gl/%.o: %.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS_BASE) -O2 -MMD -MP -c $< -o $@

obj/release/server/%.o: %.c
	@$(call MKDIR,$(@D))
	$(CC) $(CFLAGS_BASE) -O2 -MMD -MP -c $< -o $@

-include $(wildcard obj/debug/soft/*.d)
-include $(wildcard obj/debug/gl/*.d)
-include $(wildcard obj/debug/server/*.d)
-include $(wildcard obj/release/soft/*.d)
-include $(wildcard obj/release/gl/*.d)
-include $(wildcard obj/release/server/*.d)

install: release
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include/skele
	install -m 644 $(LIBSKELE_R)     $(PREFIX)/lib/libskele.a
	install -m 644 $(LIBGLSKELE_R)   $(PREFIX)/lib/libglskele.a
	install -m 644 $(LIBSKELE_SRV_R) $(PREFIX)/lib/libskeleserver.a
	cp -r include/* $(PREFIX)/include/skele/
	install -d $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/lib/libskele.a
	rm -f $(PREFIX)/lib/libglskele.a
	rm -f $(PREFIX)/lib/libskeleserver.a
	rm -rf $(PREFIX)/include/skele

clean:
	@$(call RMDIR,$(OBJ_DIR))
	@$(call RMDIR,$(BIN_DIR))
