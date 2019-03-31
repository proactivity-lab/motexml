# Makefile for libmlformat library
MAKEFILE_LOCATION := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

INSTALL_DIR ?= /usr/lib
ROOT_DIR ?= $(MAKEFILE_LOCATION)
PLATFORM ?= native
BIN_DIR ?= $(MAKEFILE_LOCATION)/../bin
LOGFILE ?= $(BIN_DIR)/$(PLATFORM)/make.log

ALL_INCS += -I $(ROOT_DIR) -I $(BIN_DIR) -I $(BIN_DIR)/$(PLATFORM)

CFLAGS = -Wall -fPIC $(OPT)
COMPILE = $(CC) $(CFLAGS) $(ALL_INCS) -c

ifeq ($(OS),Windows_NT)
	CFLAGS += -DLIBEXPORT="__declspec(dllexport)"
else
	CFLAGS += -DLIBEXPORT=""
	# For CXX
	# CFLAGS += -DLIBEXPORT="extern \"C\""
endif

LIB_OBJECTS = MLO.o MLD.o MLI.o MLE.o

LIB_VERSION_MAJOR = 1
LIB_VERSION_MINOR = 1
LIB_VERSION_PATCH = 0

LIBNAME = libmlformat
LIBNAMEVER = $(LIBNAME).so.$(LIB_VERSION_MAJOR).$(LIB_VERSION_MINOR).$(LIB_VERSION_PATCH)
LIBNAMEVER_EXT = $(LIBNAME)_extra.so.$(LIB_VERSION_MAJOR).$(LIB_VERSION_MINOR).$(LIB_VERSION_PATCH)

EXTRA_OBJECTS = MLObjects.o MLESpatial.o MLDSpatial.o MLETemporal.o MLDTemporal.o MLCompare.o MLText.o MLPrint.o

all: library

clean:
	-rm -f $(BIN_DIR)/$(PLATFORM)/*.o
	-rm -f $(LOGFILE)

dist-clean:
	-rm -Rf $(BIN_DIR)

setup:
	# Check if BIN_DIR exists and create it if it does not.
	$(shell if test -d $(BIN_DIR); then echo; else mkdir $(BIN_DIR); fi)
	$(shell if test -d $(BIN_DIR)/$(PLATFORM); then echo; else mkdir $(BIN_DIR)/$(PLATFORM); fi)

library: setup $(LIB_OBJECTS)
	$(CC) -shared -Wl,-z,defs -Wl,-soname,$(LIBNAMEVER) $(patsubst %.o, $(BIN_DIR)/$(PLATFORM)/%.o, $(LIB_OBJECTS)) -o $(BIN_DIR)/$(PLATFORM)/$(LIBNAMEVER)

extra: setup $(EXTRA_OBJECTS)
	$(CC) -shared -Wl,-soname,$(LIBNAMEVER_EXT) $(patsubst %.o, $(BIN_DIR)/$(PLATFORM)/%.o, $(EXTRA_OBJECTS)) -o $(BIN_DIR)/$(PLATFORM)/$(LIBNAMEVER_EXT)

install: library
	cp $(BIN_DIR)/$(PLATFORM)/$(LIBNAMEVER) $(INSTALL_DIR)/$(LIBNAMEVER)
	ln -s $(INSTALL_DIR)/$(LIBNAMEVER) $(INSTALL_DIR)/$(LIBNAME).so.$(LIB_VERSION_MAJOR).$(LIB_VERSION_MINOR)
	ln -s $(INSTALL_DIR)/$(LIBNAME).so.$(LIB_VERSION_MAJOR).$(LIB_VERSION_MINOR) $(INSTALL_DIR)/$(LIBNAME).so.$(LIB_VERSION_MAJOR)
	ln -s $(INSTALL_DIR)/$(LIBNAME).so.$(LIB_VERSION_MAJOR) $(INSTALL_DIR)/$(LIBNAME).so

test:
	@echo $(objects1) >> $(LOGFILE)

%.o : %.cpp
	$(COMPILE) $< -o $(BIN_DIR)/$(PLATFORM)/$@

%.o : %.c
	$(COMPILE) $< -o $(BIN_DIR)/$(PLATFORM)/$@
