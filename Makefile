RM = \rm -f
CC = gcc
CFLAGS = -O2 -Wall
INC = -I.
LIB = -lm
CFLAGS2 = -O2 -Wall `pkg-config --cflags gtk4`
LDFLAGS = `pkg-config --libs gtk4`

INSTALLDIR = $(HOME)/bin

SRC_DIR_V1 = RT3SAE02/ClientServerEnC_V1
SRC_DIR_V2 = RT3SAE02/ClientServerEnC_V2

V1_CLIENT_SRC = $(SRC_DIR_V1)/simpleClientSocket.c
V1_SERVER_SRC = $(SRC_DIR_V1)/simpleServerSocket.c
V2_SERVER_SRC = $(SRC_DIR_V2)/multipleServerSocket.c
V2_CLIENT_SRC = $(SRC_DIR_V2)/ClientGraph.c


V1_CLIENT_OBJ = $(SRC_DIR_V1)/simpleClientSocket.o
V1_SERVER_OBJ = $(SRC_DIR_V1)/simpleServerSocket.o
V2_SERVER_OBJ = $(SRC_DIR_V2)/multipleServerSocket.o
V2_CLIENT_SRC_OBJ = $(SRC_DIR_V2)/ClientGraph.o

V1_CLIENT_EXEC = $(SRC_DIR_V1)/simpleClientSocket
V1_SERVER_EXEC = $(SRC_DIR_V1)/simpleServerSocket
V2_SERVER_EXEC = $(SRC_DIR_V2)/multipleServerSocket
V2_CLIENT_EXEC = $(SRC_DIR_V2)/ClientGraph



all: $(V2_CLIENT_EXEC) $(V2_SERVER_EXEC)

$(V1_CLIENT_EXEC): $(V1_CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)


$(V2_CLIENT_EXEC): $(V2_CLIENT_SRC_OBJ)
	$(CC) $(CFLAGS2) -o $@ $^ $(LIB) $(LDFLAGS) 

$(V1_SERVER_EXEC): $(V1_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(V2_SERVER_EXEC): $(V2_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(SRC_DIR_V1)/%.o: $(SRC_DIR_V1)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(SRC_DIR_V2)/%.o: $(SRC_DIR_V2)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	$(RM) $(SRC_DIR_V1)/*.o $(SRC_DIR_V2)/*.o $(V1_CLIENT_EXEC) $(V1_SERVER_EXEC) $(V2_SERVER_EXEC) *~

.PHONY: all clean