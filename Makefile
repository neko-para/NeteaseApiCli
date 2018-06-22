all: NeteaseApi

NeteaseApi: libNeteaseApi.so

libNeteaseApi.so: crypto.o
	g++ -shared -o $@ $^ `pkg-config --libs libcrypto++` -lgmp

crypto.o: crypto.cpp crypto.h
	g++ -fPIC -c -o $@ $< `pkg-config --cflags libcrypto++`