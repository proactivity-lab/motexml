# Makefile

include $(PROLAB_COMMON_DIR)/Platform.mk

ROOT_DIR := .
BIN_DIR := ../bin/$(PLATFORM)
LOGFILE := $(BIN_DIR)/make.log

ALL_INCS += -I $(ROOT_DIR)
ALL_INCS += -I $(ROOT_DIR)/../lib
ALL_INCS += -I $(HOME)/workspace/Prolab_general/ActiveMessage/src
ALL_INCS += -I $(ROOT_DIR)/../../Smartdust

ALL_INCS += -I /home/raido/workspace/Gene/src/Shared
ALL_INCS += -I /home/raido/workspace/Gene/src/positioning/Classes
ALL_INCS += -I /home/raido/workspace/Gene/src/positioning

VPATH += $(PROLAB_COMMON_DIR)

CFLAGS = -Wall -fPIC $(OPT)
COMPILE = $(CXX) $(CFLAGS) $(ALL_INCS) -c

# Check if BIN_DIR exists and create it if it does not.
$(shell if test -d $(BIN_DIR); then echo; else mkdir $(BIN_DIR); fi)

all:

MLToMoteTCP: MLToMoteTCP.o
	$(CXX) $(CFLAGS) $(LIBS) -o $(BIN_DIR)/$@ $(patsubst %.o, $(BIN_DIR)/%.o, $<) -lpthread -lmlformat -lpositioning -lrobotcp -L $(BIN_DIR)

SERIALMLTOMOTE_OBS = SerialMLToMote.o NewMLToMote.o debug.o
SerialMLToMote: $(SERIALMLTOMOTE_OBS)
	$(CXX) $(CFLAGS) $(LIBS) -o $(BIN_DIR)/$@ $(patsubst %.o, $(BIN_DIR)/%.o, $(SERIALMLTOMOTE_OBS)) -lpthread -lmlformat -lcppactivemessage -L $(BIN_DIR)

UDPMLTOMOTE_OBS = UdpMLToMote.o NewMLToMote.o debug.o
UdpMLToMote: $(UDPMLTOMOTE_OBS)
	$(CXX) $(CFLAGS) $(LIBS) -o $(BIN_DIR)/$@ $(patsubst %.o, $(BIN_DIR)/%.o, $(UDPMLTOMOTE_OBS)) -lpthread -lmlformat -lcppactivemessage -L $(BIN_DIR)

clean:
	-rm -f $(BIN_DIR)/*.o
	-rm -f $(LOGFILE)

dist-clean:
	-rm -f $(BIN_DIR)/*

%.o : %.cpp
	$(COMPILE) $< -o $(BIN_DIR)/$@

%.o : %.c
	$(COMPILE) $< -o $(BIN_DIR)/$@
