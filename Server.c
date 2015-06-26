#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h> 

#define PORT 7000
#define BUFFER_SIZE 256
#define MAX_FILENAME_SIZE 256
#define LISTEN_QUEUE_SIZE 5
#define MAX_THREAD_COUNT 5

#define TRUE 1
#define FALSE 0

void* thread_function(void *param)
{
	char filepath[MAX_FILENAME_SIZE];
	unsigned char buffer[BUFFER_SIZE];
	int bytes_send = 0, bytes_read = 0, client_id = (int)param;
	FILE *file;

	memset(buffer, 0, sizeof(buffer));

	bytes_read = read(client_id, filepath, sizeof(filepath) - 1);
	if (bytes_read == -1) {
		printf("ERROR on reading filepath.\n");
		close(client_id);
		return;
	}

	filepath[bytes_read] = 0;
	if (access(filepath, F_OK) == -1) {
		printf("File not found.\n");
		close(client_id);
		return;
	}

	file = fopen(filepath, "rb");
	if (file == NULL) {
		printf("File open error.\n");
		close(client_id);
		return;
	}

	while (TRUE) {
		bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
	
		if (bytes_read > 0) {
			bytes_send = write(client_id, buffer, bytes_read);
			if (bytes_send < bytes_read)
				printf("Error sending file.\n");
		}

		if (bytes_read < BUFFER_SIZE) {
			if (feof(file))
				printf("File was sent.\n");
			if (ferror(file))
				printf("Error reading from file.\n");
			break;
		}
	}
	fclose(file);
	close(client_id);	
}
