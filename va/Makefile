#GUILE_INCLUDES=/new_devs/usr/local/guile-new/include/guile/3.0/
GUILE_INCLUDES=/usr/include/guile/3.0/
GUILE_LIBS=guile-3.0
#GUILE_LIBS_DIR=-L/new_devs/usr/local/guile-new/lib
GUILE_LIBS_DIR=

#OTHER_INCLUDES=-I/new_devs/usr/local/shared/include -I/new_devs/usr/local/shared/include/openssl  -I/new_devs/usr/local/shared/include/curl  -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/websocketpp
OTHER_INCLUDES=-I/new_devs/usr/local/shared/include 
# -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/websocketpp
OTHER_LIBS=-L/new_devs/usr/local/shared/lib

OBJS= \
	internalsonda.o \
	lb_user.o \
	listaeventi.o \
	main.o \
	network.o \
	thfunct.o \
	lex.yy.o \
	parser.tab.o 	\
	list_sockets.o  \
	rulemgr.o \
	va_commands.o \
	va_scanner.o \
	va_parser.o \
	interpreter.o \
	command.o \
	VAengine.o \
	VA_ast_parsing.o \
	VA_utils.o \
	times.o


SRC= \
	lb_user.cpp \
	listaeventi.cpp \
	main.cpp \
	network.cpp \
	thfunct.cpp \
	lex.yy.cpp \
	parser.tab.cpp 	\
	list_sockets.cpp  \
	rulemgr.cpp \
	va_commands.cpp \
	va_scanner.cpp \
	va_parser.cpp \
	interpreter.cpp \
	command.cpp \
	VAengine.cpp \
	VA_ast_parsing.cpp \
	VA_utils.cpp \
	internalsonda.cpp \
	times.cpp


CPP=gcc
CXX=g++
CC=gcc
LD=g++

CPPFLAGS=
CFLAGS=
CXXFLAGS= -std=c++17

#NEWDEVS=/new_devs/usr/local/
#LIBEVENT=$(NEWDEVS)/libevent
#OPENSSL=$(NEWDEVS)/openssl

INCLUDES=$(OTHER_INCLUDES) -I. -I/volume1/sources/NEW_DEVS/LISP/mtfa_libraries/ -I/usr/local/include -I/usr/include/mysql -I$(GUILE_INCLUDES) -I/usr/include/hs

LIBS=   $(OTHER_LIBS) -L/volume1/sources/NEW_DEVS/LISP/mtfa_libraries/ -lmtfa \
        -lcurl -lgmp -lgmpxx -lsqlite3 \
        -lssl -lcrypto -levent -levent_core -levent_extra -levent_pthreads -L/usr/lib/x86_64-linux-gnu \
         -levent_openssl  \
	-lpcre2-8 -lmagic \
	$(GUILE_LIBS_DIR) -l$(GUILE_LIBS) \
	-lrlpvalue  -lunivalue -lsecp256k1 -lmysqlclient \
	/usr/lib/x86_64-linux-gnu/libhs.a \
	-lz -ldl -lpthread -lreadline -lcurses -ltermcap -lrt

        #-ltls \
	#/usr/lib/x86_64-linux-gnu/libtcmalloc_minimal.so.4

ifdef RELEASE
	OPT=-pthread -O3 -g0 -Wmissing-declarations -fpermissive
	#-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 
else
	OPT=-fstack-protector -pthread -O0 -g -Wmissing-declarations -fpermissive
	#-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 
endif

all: va

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

va: $(OBJS) /volume1/sources/NEW_DEVS/LISP/mtfa_libraries/libmtfa.a
	$(CXX) $(CXXFLAGS) ${OPT} -o va -Wl,--copy-dt-needed-entries $(OBJS) $(LIBS)

.cpp.o:
	$(CXX)  ${OPT} -c $(CXXFLAGS) $(INCLUDES) $< -o $@
	$(CXX) $(INCLUDES) -MM $(CXXFLAGS) $*.cpp > $*.d

.c.o:
	$(CC)  ${OPT} -c $(CFLAGS) $(INCLUDES) $< -o $@
	$(CC) $(INCLUDES) -MM $(CFLAGS) $*.c > $*.d

parser.tab.cpp: parser.y
	bison -d parser.y -o parser.tab.cpp

parser.tab.hpp: parser.y
	bison -d parser.y -o parser.tab.cpp

lex.yy.cpp: parser.l parser.tab.hpp
	flex --full -o lex.yy.cpp parser.l

va_scanner.cpp: va_scanner.l va_parser.y interpreter.cpp command.cpp command.h interpreter.h va_scanner.h
	flex -o va_scanner.cpp va_scanner.l

va_parser.cpp: va_scanner.l va_parser.y interpreter.cpp command.cpp command.h interpreter.h va_scanner.h
	bison -o va_parser.cpp va_parser.y -r all

va_parser.hpp: va_parser.cpp
va_scanner.cpp: va_parser.cpp
lb_user.cpp: va_parser.hpp

clean:
	/bin/rm -f $(OBJS) *.d va parser.tab.cpp parser.tab.hpp lex.yy.cpp va_scanner.cpp va_parser.cpp va_parser.hpp location.hh position.hh stack.hh

