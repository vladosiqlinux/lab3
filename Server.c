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
	int bytes_send = 0, bytes_read = 0;
	long long client_id = (long long)param;
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

int main(int argc, char* argv[])
{
	int socket_id = 0, opt = 1;
	long long client_id = 0;
	struct sockaddr_in server_socket;

#ifdef THREAD	
	pthread_t threads[MAX_THREAD_COUNT] = {};
	int i = 0;
#elif defined(PROCESS)
	pid_t processId = 0;
#endif
	
	if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("ERROR opening socket.\n");
		return 1;
	}

	server_socket.sin_family = AF_INET;
	server_socket.sin_addr.s_addr = INADDR_ANY;
	server_socket.sin_port = htons(PORT);

	if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
    		printf("Socket options weren't set.\n");
		return 1;
	}

	if ((bind(socket_id, (struct sockaddr *)&server_socket, sizeof(server_socket))) == -1) {
		printf("ERROR on binding.\n");
		return 1;
	}

	if (listen(socket_id, LISTEN_QUEUE_SIZE) == -1) {
		printf("ERROR on listening.\n");
		return -1;
	}

	while (TRUE) {
		client_id = accept(socket_id, NULL, (socklen_t *)SOCK_CLOEXEC);

		if (client_id < 0) {
			printf("ERROR on accept client.\n");
			continue;
		}
#ifdef THREAD
		for (i = 0; i < MAX_THREAD_COUNT; ++i)
			if ((threads[i]==0) || (pthread_kill(threads[i], 0) != ESRCH))
				break;

		if (i >= MAX_THREAD_COUNT) {
			printf("No free threads.\n");
			continue;
		}

		if (pthread_create(&threads[i], NULL, thread_function, (void*)client_id)) {
			printf("Thread wasn't created.\n");
			continue;
		}
		printf("Thread was created.\n");
#elif defined(PROCESS)
		switch (processId = fork()) {
		case -1:
			printf("Process creation failed.\n");
			break;
		case 0:
			thread_function((void*)client_id);
			return 0;
		default:
		  	close(client_id);
			break;
		}
#else
		thread_function((void*)client_id);
#endif			
	}

	return 0;
}
