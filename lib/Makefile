# Makefile

ROOT_DIR = .
PLATFORM ?= native
BIN_DIR = ../bin
LIB_DIR ?= /opt/prolib
LOGFILE = $(BIN_DIR)/$(PLATFORM)/make.log

ALL_INCS += -I $(ROOT_DIR) -I $(BIN_DIR) -I $(BIN_DIR)/$(PLATFORM)

CFLAGS = -Wall -fPIC $(OPT)
COMPILE = $(CXX) $(CFLAGS) $(ALL_INCS) -c

ifeq ($(OS),Windows_NT)
	CFLAGS += -DLIBEXPORT="__declspec(dllexport)"
else
	CFLAGS += -DLIBEXPORT="extern \"C\""
endif

LIB_OBJECTS = MLO.o MLD.o MLI.o MLE.o

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
	$(CXX) -shared -Wl,-z,defs -Wl,-soname,libmlformat.so $(patsubst %.o, $(BIN_DIR)/$(PLATFORM)/%.o, $(LIB_OBJECTS)) -o $(BIN_DIR)/$(PLATFORM)/libmlformat.so

extra: setup $(EXTRA_OBJECTS)
	$(CXX) -shared -Wl,-soname,libmlformat_extra.so $(patsubst %.o, $(BIN_DIR)/$(PLATFORM)/%.o, $(EXTRA_OBJECTS)) -o $(BIN_DIR)/$(PLATFORM)/libmlformat_extra.so

install: library
	cp $(BIN_DIR)/$(PLATFORM)/libmlformat.so $(LIB_DIR)/libmlformat.so

test:
	@echo $(objects1) >> $(LOGFILE)

%.o : %.cpp
	$(COMPILE) $< -o $(BIN_DIR)/$(PLATFORM)/$@

%.o : %.c
	$(COMPILE) $< -o $(BIN_DIR)/$(PLATFORM)/$@
