EXFLAG=-g -Wall -Wextra
PREFIX=/usr/local

all: NeteaseApi

clean:
	-rm -f *.o NeteaseApi libNeteaseApi.so

install: all
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/include
	mkdir -p $(PREFIX)/lib
	install -m 755 NeteaseApi $(PREFIX)/bin/NeteaseApi
	install -m 644 NeteaseApi.h $(PREFIX)/include/NeteaseApi.h
	install -m 644 libNeteaseApi.so $(PREFIX)/lib/libNeteaseApi.so

NeteaseApi: main.cpp NeteaseApi.h libNeteaseApi.so
	g++ $(EXFLAG) -std=c++11 -o $@ main.cpp -L. -Wl,--rpath=. -lNeteaseApi `pkg-config --cflags --libs libcurl`

libNeteaseApi.so: crypto.o NeteaseApi.o
	g++ $(EXFLAG) -shared -o $@ $^ `pkg-config --libs libcrypto++` -lgmp

crypto.o: crypto.cpp crypto.h
	g++ $(EXFLAG) -std=c++11 -fPIC -c -o $@ $< `pkg-config --cflags libcrypto++`

NeteaseApi.o: NeteaseApi.cpp NeteaseApi.h
	g++ $(EXFLAG) -std=c++11 -fPIC -c -o $@ $<
