thread:
	gcc Client.c -o Client.bin
	gcc Server.c -DTHREAD -o Server.bin -lpthread
process:
	gcc Client.c -o Client.bin
	gcc Server.c -DPROCESS -o Server.bin
clean:
	rm -rf *.bin
