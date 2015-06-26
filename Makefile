all:
	gcc Client.c -o Client.bin
	gcc Server.c -o Server.bin
thread:
	gcc Client.c -o Client.bin
	gcc Server.c -DTHREAD -o Server.bin -lpthread
process:
	gcc Client.c -o Client.bin
	gcc Server.c -DPROCESS -o Server.bin -lpthread
clean:
	rm -rf *.bin
