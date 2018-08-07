CLIENT := client
SERVER := server
DAEMON := daemon
EXECUTABLE = $(CLIENT) $(SERVER) $(DAEMON)

CJSON_DIR = /study/repository/linux/linux-c/workspace/mini_servie_protocal/cJSON-1.7.6/install
PROVIDER_DIR = ../provider
COMM_DIR = ../provider/comm


CC := gcc
CFLAGS  := -Wall -O2 -I$(CJSON_DIR)/include/cjson/ -I$(PROVIDER_DIR) -I$(COMM_DIR)
LDFLAGS += -L$(CJSON_DIR)/lib -lcjson 
LDFLAGS += -Wl,-rpath,$(CJSON_DIR)/lib -lpthread


SOURCE = $(wildcard $(PROVIDER_DIR)/*.c)
SOURCE += $(wildcard $(COMM_DIR)/*.c)


.PHONY : clean all

all : $(EXECUTABLE)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

#
#
CLIENT_OBJS := $(patsubst %.c, %.o, $(SOURCE))
CLIENT_OBJS += $(CLIENT).o

$(CLIENT):$(CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#
#
SERVER_OBJS := $(patsubst %.c, %.o, $(SOURCE))
SERVER_OBJS += $(SERVER).o

$(SERVER):$(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#
#
DAEMON_OBJS := $(patsubst %.c, %.o, $(SOURCE))
DAEMON_OBJS += $(DAEMON).o

$(DAEMON):$(DAEMON_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#
#
OBJS = $(CLIENT_OBJS) $(SERVER_OBJS) $(DAEMON_OBJS)



clean :
	@rm -f $(EXECUTABLE) $(OBJS)

