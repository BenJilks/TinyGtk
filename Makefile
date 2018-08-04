 
all:
	gcc `pkg-config --cflags gtk+-3.0` *.c -shared -fPIC -o TinyGtk/TinyGtk.so `pkg-config --libs gtk+-3.0`
