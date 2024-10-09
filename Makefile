all:
	clang main.c react.c -o creat -ggdb

run: all
	./creat

serve:
	gcc server.c -o aaaaaaserver
	./aaaaaaserver

clean:
	rm -f aaaaaaserver creat lol.html index.html
