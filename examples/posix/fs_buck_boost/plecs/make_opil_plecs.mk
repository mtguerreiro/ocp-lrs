OPIL      := ../..
BUILD_DIR := plecs/build
STYPES_DIR  := .
HOST_COMM_SOCK_SERVER_IP := 131.246.75.241
HOST_COMM_SOCK_SERVER_PORT := 8090

ifeq ($(OS),Windows_NT)
	PLAT := win
    BUILD_DIR_WIN := $(subst /,\,${BUILD_DIR})
	DIR_GUARD := if not exist ${BUILD_DIR_WIN} mkdir ${BUILD_DIR_WIN}
	SOCK_LIB  := ws2_32
	EXT_LIB   := .dll
	COMM_DIR  := win
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
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
	gcc -fPIC -O2 -c ${OPIL}/comm/$(COMM_DIR)/hostCommSock.c -I${OPIL}/comm/ -o ${BUILD_DIR}/hostCommSock.o -DHOST_COMM_SOCK_SERVER_IP=\"${HOST_COMM_SOCK_SERVER_IP}\" -DHOST_COMM_SOCK_SERVER_PORT=${HOST_COMM_SOCK_SERVER_PORT}
	gcc -fPIC -O2 -c ${OPIL}/simif/simif.c -I${STYPES_DIR} -I${OPIL}/ -o ${BUILD_DIR}/simif.o
	gcc -fPIC -O2 -c ${OPIL}/opilhost.c -o ${BUILD_DIR}/opilhost.o
	gcc -fPIC -O2 -c ${OPIL}/sw/plecs/opil_plecs.c -I${STYPES_DIR} -I${OPIL}/ -o ${BUILD_DIR}/opil_plecs.o
ifeq ($(PLAT),win)
	gcc -L. -Wl,-rpath=. -shared -o ${BUILD_DIR}/libopil_plecs${EXT_LIB} ${BUILD_DIR}/opil_plecs.o ${BUILD_DIR}/hostCommSock.o ${BUILD_DIR}/simif.o ${BUILD_DIR}/opilhost.o -l${SOCK_LIB}
else
	gcc -L. -Wl,-rpath=. -shared -o ${BUILD_DIR}/libopil_plecs${EXT_LIB} ${BUILD_DIR}/opil_plecs.o ${BUILD_DIR}/hostCommSock.o ${BUILD_DIR}/simif.o ${BUILD_DIR}/opilhost.o
endif

