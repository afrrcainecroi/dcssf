#GUILE_INCLUDES=/new_devs/usr/local/guile-new/include/guile/3.0/
GUILE_INCLUDES=/usr/include/guile/3.0/
GUILE_LIBS=guile-3.0
#GUILE_LIBS_DIR=-L/new_devs/usr/local/guile-new/lib
GUILE_LIBS_DIR=

#OTHER_INCLUDES=-I/new_devs/usr/local/shared/include -I/new_devs/usr/local/shared/include/openssl  -I/new_devs/usr/local/shared/include/curl  -I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/websocketpp
OTHER_INCLUDES=-I/new_devs/usr/local/shared/include
#-I/volume1/sources/NEW_DEVS/GUILE-AND-LIBRARIES/websocketpp
OTHER_LIBS=-L/new_devs/usr/local/shared/lib

INCLUDES=$(OTHER_INCLUDES) -I. -I/usr/local/include -I/usr/include/mysql -I$(GUILE_INCLUDES)

OBJS= \
	ConfigFile.o \
	ULL_Rng.o \
	http_parser.o \
	mtfa_fs3.o \
	mtfa_ph.o \
	mtfa_qt.o \
	mtfafs.o \
	mtfabrg.o \
	smallmat.o \
	mtfa_utils.o \
	base64.o \
	mtfa_find.o \
	mtfa_fuzzyfind.o \
	mtfa_db.o \
	crc.o \
	runextprog.o \
	sendmail.o \
	mtfa_shared_mutex.o \
	NumberTheory.o \
	BitBuffer.o \
	QrCode.o \
	QrSegment.o \
	setunion.o \
	

	#netfilter_queue.o \




SRC=\
	ConfigFile.cpp \
	ULL_Rng.cpp \
	http_parser.c \
	mtfa_fs3.cpp \
	mtfa_ph.cpp \
	mtfa_qt.cpp \
	mtfafs.cpp \
	mtfabrg.cpp \
	smallmat.cpp \
	mtfa_utils.cpp \
	base64.cpp \
	mtfa_find.cpp \
	mtfa_fuzzyfind.cpp \
	mtfa_db.cpp \
	crc.cpp \
	runextprog.cpp \
	sendmail.cpp \
	mtfa_shared_mutex.cpp \
	NumberTheory.cpp \
	BitBuffer.cpp \
	QrCode.cpp \
	QrSegment.cpp \
	setunion.cpp \
	

	#netfilter_queue.cpp \


#LIBS=   -lcurl -lgmp -lgmpxx -lsqlite3 -lmysqlclient -L$(LIBEVENT)/lib -L$(OPENSSL)/lib -lssl -lcrypto -levent -levent_core -levent_extra -levent_pthreads \

LIBS=   -lcurl -lgmp -lgmpxx -lsqlite3 -lssl -lcrypto -levent -levent_core -levent_extra -levent_pthreads -L/usr/lib/x86_64-linux-gnu   \
        -levent_openssl  \
	-lmagic \
        $(GUILE_LIBS_DIR) -l$(GUILE_LIBS) \
        $(OTHER_LIBS) -lrlpvalue  -lunivalue -lsecp256k1 \
        -lmysqlclient \
        -lz -ldl -lpthread -lreadline -lcurses -ltermcap -lrt -lpcre2-8 -ltre
        
        #

#-lnfnetlink -lnetfilter_queue -lmnl 
#/usr/lib/x86_64-linux-gnu/libtcmalloc_minimal.so.4

CPP=g++
CXX=g++
CC=gcc
LD=g++

ifdef RELEASE
	OPT= -pthread -g -Ofast -Wmissing-declarations -fpermissive -march=native
	#-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free  
else
	OPT= -pthread -g -O0 -Wmissing-declarations -fpermissive -march=native
	#-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 
endif

CPPFLAGS=$(INCLUDES)

CFLAGS= $(CPPFLAGS)
CXXFLAGS=$(CPPFLAGS) -std=c++17

LDFLAGS= $(OPT)

all: libmtfa.a check

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

libmtfa.a: $(OBJS)
	ar -r libmtfa.a $(OBJS)

check: main.o libmtfa.a 
	$(LD) $(OPT) -fPIC -o check $(CXXFLAGS) main.o $(OBJS) $(LIBS)

.c.o:
	$(CC) $(OPT) -fPIC -c $(CFLAGS) $(INCLUDES) $< -o $@
	$(CC) -MM $(CFLAGS) $*.c > $*.d

.cpp.o:
	$(CXX) $(OPT) -fPIC -c $(CXXFLAGS) $(INCLUDES) $< -o $@
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d


clean:
	rm -f $(OBJS) libmtfa.a check main.o *.d

