OPIL := ../../../../../opil
BUILD_DIR := build
OPIL_PORT ?= 8090
CFLAGS += -fPIC -O2 -DHOST_COMM_SOCK_SERVER_PORT=$(OPIL_PORT)

ifeq ($(OS),Windows_NT)
	RM := del /q
	PLAT := win
	DIR_GUARD := if not exist ${BUILD_DIR} mkdir ${BUILD_DIR}
	SOCK_LIB  := ws2_32
	EXT_LIB   := .dll
	COMM_DIR  := win
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		RM := rm -rf
		PLAT := linux
		DIR_GUARD := mkdir -p ${BUILD_DIR}
		EXT_LIB   := .so
		COMM_DIR  := linux
	else
		$(error Unsupported platform: $(UNAME_S))
	endif
endif

all: ${PLAT}

$(PLAT): ${OPIL}/sw/plecs/opil_plecs.c
	${DIR_GUARD}
	gcc $(CFLAGS) -c ${OPIL}/comm/$(COMM_DIR)/hostCommSock.c -I${OPIL}/comm/ -o ${BUILD_DIR}/hostCommSock.o
	gcc $(CFLAGS) -c ${OPIL}/simif/simif.c -I. -I../ -I${OPIL}/ -o ${BUILD_DIR}/simif.o
	gcc $(CFLAGS) -c ${OPIL}/opilhost.c -o ${BUILD_DIR}/opilhost.o
	gcc $(CFLAGS) -c ${OPIL}/sw/plecs/opil_plecs.c -I. -I../ -I${OPIL}/ -o ${BUILD_DIR}/opil_plecs.o
ifeq ($(PLAT),win)
	gcc -L. -Wl,-rpath=. -shared -o ${BUILD_DIR}/libopil_plecs${EXT_LIB} ${BUILD_DIR}/opil_plecs.o ${BUILD_DIR}/hostCommSock.o ${BUILD_DIR}/simif.o ${BUILD_DIR}/opilhost.o -l${SOCK_LIB}
else
	gcc -L. -Wl,-rpath=. -shared -o ${BUILD_DIR}/libopil_plecs${EXT_LIB} ${BUILD_DIR}/opil_plecs.o ${BUILD_DIR}/hostCommSock.o ${BUILD_DIR}/simif.o ${BUILD_DIR}/opilhost.o
endif

clean:
	${RM} ${BUILD_DIR}
