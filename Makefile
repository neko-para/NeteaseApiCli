EXFLAG=-g -Wall -Wextra

all: NeteaseApi

NeteaseApi: main.cpp NeteaseApi.h libNeteaseApi.so
	g++ $(EXFLAG) -o $@ main.cpp -L. -Wl,--rpath=. -lNeteaseApi

libNeteaseApi.so: crypto.o util.o NeteaseApi.o
	g++ $(EXFLAG) -shared -o $@ $^ `pkg-config --libs libcrypto++ libcurl` -lgmp

crypto.o: crypto.cpp crypto.h
	g++ $(EXFLAG) -fPIC -c -o $@ $< `pkg-config --cflags libcrypto++`

util.o: util.cpp crypto.h util.h
	g++ $(EXFLAG) -fPIC -c -o $@ $< `pkg-config --cflags libcurl`

NeteaseApi.o: NeteaseApi.cpp NeteaseApi.h
	g++ $(EXFLAG) -fPIC -c -o $@ $< `pkg-config --cflags libcurl`