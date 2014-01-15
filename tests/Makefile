# Makefile

include $(PROLAB_COMMON_DIR)/Platform.mk

ROOT_DIR := .
BIN_DIR := ../bin/$(PLATFORM)
LOGFILE := $(BIN_DIR)/make.log

ALL_INCS += -I $(ROOT_DIR)
ALL_INCS += -I $(ROOT_DIR)/../lib
ALL_INCS += -I $(ROOT_DIR)/../../ActiveMessage/src
ALL_INCS += -I $(ROOT_DIR)/../../Smartdust

VPATH += $(PROLAB_COMMON_DIR)

CFLAGS = -Wall -fPIC $(OPT)
COMPILE = $(CXX) $(CFLAGS) $(ALL_INCS) -c

# Check if BIN_DIR exists and create it if it does not.
$(shell if test -d $(BIN_DIR); then echo; else mkdir $(BIN_DIR); fi)

all:

dist-clean:
	-rm -f $(BIN_DIR)/*

clean:
	-rm -f $(BIN_DIR)/*.o

MLFromMote: MLFromMote.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/MLFromMote.o -lpthread -lML -lpositioning -L $(LIB_DIR)

MLCompareTest: MLCompareTest.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< -lmlformat -L $(BIN_DIR)

MLEncodeTest: MLEncodeTest.o debug.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< $(BIN_DIR)/debug.o -lmlformat -L $(BIN_DIR)

TemporalTest: TemporalTest.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< -lmlformat -L $(BIN_DIR)

SpatialTest: SpatialTest.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< -lmlformat -L $(BIN_DIR)

MLITest: MLITest.o debug.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< $(BIN_DIR)/debug.o -lmlformat -lmlformat_extra -L $(BIN_DIR)
	
CopyTest: CopyTest.o debug.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< $(BIN_DIR)/debug.o -lmlformat -lmlformat_extra -L $(BIN_DIR)

MLDeleteTest: MLDeleteTest.o debug.o
	$(CXX) -o $(BIN_DIR)/$@ $(BIN_DIR)/$< $(BIN_DIR)/debug.o -lmlformat -lmlformat_extra -L $(BIN_DIR)

%.o : %.cpp
	$(COMPILE)  $< -o $(BIN_DIR)/$@

%.o : lib/%.cpp
	$(COMPILE)  $< -o $(BIN_DIR)/$@
