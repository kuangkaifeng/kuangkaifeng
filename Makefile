
#
# Makefile
#-L/home/gec/freetype-2.12.1/tmp/lib -I/home/gec/freetype-2.12.1/tmp/include/freetype2
CC = arm-linux-gcc
#添加g++编译器	  
GCC=arm-linux-g++  

LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -O3 -g0   -I$(LVGL_DIR)/ -Wall  -std=gnu99
LDFLAGS ?=  -L./freetype/lib  -I./freetype/include/freetype2  -lm  -lfreetype -lz
BIN = demo


#Collect the files to compile
MAINSRC = ./main.cpp  

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk

CSRCS +=$(LVGL_DIR)/mouse_cursor_icon.c 

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.cpp=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES


all: default

#利用gcc 编译 C 代码  
%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@     $(LDFLAGS)
	@echo "CC $<"


#利用G++ 编译 C++ 代码 
main.o: main.cpp
	@$(GCC)  $(CFLAGS) -c $< -o $@     $(LDFLAGS)
	@echo "CC $<"


default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(GCC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)

