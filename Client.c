#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 7000
#define MAX_FILEPATH_SIZE 256
#define BUF_SIZE 256

int main()
{
	char filepath[MAX_FILEPATH_SIZE];
	int bytes_received = 0, socket_id = 0;
	char buffer[BUF_SIZE];
	struct sockaddr_in server_socket;
	FILE *file;
	
	memset(buffer, 0, sizeof(buffer));
	
	printf("Input file path: ");
	scanf("%s", filepath);
	
	if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("ERROR opening socket.\n");
		return 1;
	}
	
	server_socket.sin_family = AF_INET;
	server_socket.sin_port = htons(PORT); 
	server_socket.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	
	if (connect(socket_id, (struct sockaddr *)&server_socket, sizeof(server_socket)) == -1) {
		printf("ERROR connecting.\n");
		return 1;
	}
	
	if (send(socket_id, filepath, strlen(filepath), 0) == -1) {
		printf("Send failed.\n");
		return 1;
	}

	file = fopen("output.txt", "wb");
	while ((bytes_received = read(socket_id, buffer, BUF_SIZE)) > 0) {
		fwrite(buffer, 1, bytes_received, file);
	}
	
	if (bytes_received < 0)
		printf("Reading failed.\n");
	
	printf("File reading finished.\n");
	fclose(file);
	close(socket_id);
	return 0;	
}
