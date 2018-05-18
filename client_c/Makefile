

EXECUTABLE := client_c
  

CC := gcc
CFLAGS := -Wall -O2 -I/study/repository/linux/linux-c/workspace/mini_servie_protocal/cJSON-1.7.6/install/include/cjson/
LDFLAGS+= -L/study/repository/linux/linux-c/workspace/mini_servie_protocal/cJSON-1.7.6/install/lib -lcjson 
LDFLAGS+= -Wl,-rpath,/study/repository/linux/linux-c/workspace/mini_servie_protocal/cJSON-1.7.6/install/lib

SOURCE := $(wildcard *.c)

OBJS := $(patsubst %.c, %.o, $(SOURCE))
  
all : $(EXECUTABLE)

$(EXECUTABLE) : $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) -o $(EXECUTABLE) $(LDFLAGS)
  
.PHONY : clean
  
clean :
	@rm -f $(EXECUTABLE) $(OBJS) *.o

