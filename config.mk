SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

GL_MAJOR ?= 3
GL_MINOR ?= 3

ENGINE_SRCS = \
	src/skele.c \
	$(CLOCK_SRC)

DEBUG_FLAGS ?=
