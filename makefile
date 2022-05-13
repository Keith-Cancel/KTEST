CC       := gcc
CFLAGS   := -std=c11 -Os -fno-ident -falign-functions -Werror -D_POSIX_C_SOURCE=200809
LDFLAGS  :=
LDLIBS   :=
INCLUDES := -I./include
C_WARN   := -pedantic-errors -Wall -Wextra -Wparentheses -Wdouble-promotion \
-Warith-conversion -Wduplicated-branches -Wduplicated-cond -Wshadow \
-Wunsafe-loop-optimizations -Wbad-function-cast -Wunsuffixed-float-constants \
-fanalyzer

SRC_DIR := src
OBJ_DIR := obj
LIB_DIR := lib

# For gcc MinGW compiler when using Msys2 for instance
ifeq ($(OS), Windows_NT)
EXE_EXT :=.exe
endif

LIB := $(LIB_DIR)/libktest.a
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJ) | $(LIB_DIR)
	ar -crs $@ $^
	cp include/ktest.h $(LIB_DIR)/ktest.h

# https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html
# -MMD generates a *.d file for each source file so we can have header
# dependecies
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c -MMD $(C_WARN) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rv $(LIB_DIR) $(OBJ_DIR)

# include all the new rules that depend on the header
-include $(OBJ:.o=.d)