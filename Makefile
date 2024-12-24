RM = \rm -f
CC = gcc
CFLAGS = -O2 -Wall
INC = -I.
LIB = -lm -lssh -lpthread

INSTALLDIR = $(HOME)/bin

SRC_DIR_V2 = RT3SAE02/ClientServerEnC_V2

V2_SERVER_SRC = $(SRC_DIR_V2)/server.c $(SRC_DIR_V2)/unSecured_server.c
#V2_CLIENT_SRC = $(SRC_DIR_V2)/SSH_Client.c

V2_SERVER_OBJ = $(V2_SERVER_SRC:.c=.o)
#V2_CLIENT_OBJ = $(V2_CLIENT_SRC:.c=.o)

V2_SERVER_EXEC = $(SRC_DIR_V2)/server
#V2_CLIENT_EXEC = $(SRC_DIR_V2)/SSH_Client

all: $(V2_SERVER_EXEC) #$(V2_CLIENT_EXEC)

$(V2_SERVER_EXEC): $(V2_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)
	$(RM) $(SRC_DIR_V2)/*.o

#$(V2_CLIENT_EXEC): $(V2_CLIENT_OBJ)
#	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(SRC_DIR_V2)/%.o: $(SRC_DIR_V2)/%.c
	$(CC) $(INC) -c $< -o $@

clean:
	$(RM) $(SRC_DIR_V2)/*.o $(V2_SERVER_EXEC) $(V2_CLIENT_EXEC) *~

.PHONY: all clean