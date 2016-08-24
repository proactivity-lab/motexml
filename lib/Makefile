# Makefile

ROOT_DIR = .
PLATFORM ?= native
BIN_DIR = ../bin
LIB_DIR ?= /opt/prolib
LOGFILE = $(BIN_DIR)/$(PLATFORM)/make.log

ifndef PROLAB_COMMON_DIR
    $(error Please define PROLAB_COMMON_DIR containing Makefiles and type headers)
endif

include $(PROLAB_COMMON_DIR)/Platform.mk

ALL_INCS += -I $(ROOT_DIR) -I $(BIN_DIR) -I $(BIN_DIR)/$(PLATFORM)

CFLAGS = -Wall -fPIC $(OPT)
COMPILE = $(CXX) $(CFLAGS) $(ALL_INCS) -c

LIB_OBJECTS = MLO.o MLD.o MLI.o MLE.o

EXTRA_OBJECTS = MLObjects.o MLESpatial.o MLDSpatial.o MLETemporal.o MLDTemporal.o MLCompare.o MLText.o MLPrint.o

all: types library

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
	$(CXX) -shared -Wl,-z,defs -Wl,-soname,libmlformat.so $(patsubst %.o, $(BIN_DIR)/$(PLATFORM)/%.o, $(LIB_OBJECTS)) -o $(BIN_DIR)/$(PLATFORM)/libmlformat.so

extra: setup $(EXTRA_OBJECTS)
	$(CXX) -shared -Wl,-soname,libmlformat_extra.so $(patsubst %.o, $(BIN_DIR)/$(PLATFORM)/%.o, $(EXTRA_OBJECTS)) -o $(BIN_DIR)/$(PLATFORM)/libmlformat_extra.so

install: library
	cp $(BIN_DIR)/$(PLATFORM)/libmlformat.so $(LIB_DIR)/libmlformat.so

types: setup
	motexml-generate --input $(ROOT_DIR)/../dt_types.txt --output $(BIN_DIR)/dt_types.h

test:
	@echo $(objects1) >> $(LOGFILE)

%.o : %.cpp
	$(COMPILE) $< -o $(BIN_DIR)/$(PLATFORM)/$@

%.o : %.c
	$(COMPILE) $< -o $(BIN_DIR)/$(PLATFORM)/$@
