#include <stdio.h>
#include <string.h>
#include "mpi.h"

int MYMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
{
	int rank, size, tag, next, last = 0;
		MPI_Status status;

	MPI_Comm_rank(communicator, &rank); // task id
	MPI_Comm_size(communicator, &size); //size

	//If the rank is root, start sending to 'next'. Else receive from 'last' and send to 'next'
	if (rank == root) {
		next = (rank + 1) % size;
		MPI_Send(buffer, count, datatype, next, tag, communicator);
	} else {
		last = mod((rank - 1), size);
		next = (rank + 1) % size;
		MPI_Recv(buffer, count, datatype, last, tag, communicator, &status);
		MPI_Send(buffer, count, datatype, next, tag, communicator);
	}

	return MPI_SUCCESS;
}

//Because % in C calculates the remainder it does not work with negative numbers.
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int main(int argc, char **argv)
{
	const int maxBuffer = 32;
	char buffer[maxBuffer]; //Buffer
	int rank, size, conn, sender;
	int messagesize = maxBuffer;
	const int root = 0; //can be changed to another value smaller then size.
	char sendmessage[] = "Hello, world!";

	//Setup MPI, if an error occured exit.
	conn = MPI_Init(&argc, &argv);
	if (conn != MPI_SUCCESS) {
		fprintf(stderr, "Unable to set up MPI\n");
		MPI_Abort(MPI_COMM_WORLD, conn);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Only the root will broadcast a message
	if (rank == root)
	{
		messagesize = strlen(sendmessage) + 1;
		//Check if the message is not bigger then the buffer
		if (messagesize > maxBuffer) {
			fprintf(stderr, "Messagesize is to big for the current buffer. Aborting!\n");
			MPI_Abort(MPI_COMM_WORLD, conn);
		}
		strcpy(buffer, sendmessage);
		printf("I (task %d of %d total tasks) will broadcast: %s\n", rank, size-1, buffer);
	}

	//Test to check if the buffer is really empty before receiving anything
	/*printf("My (%d) buffer contains %d characters (%s)\n", rank, strlen(buffer), buffer);*/

	//The root will send it's buffer the other processes will only receive.
	MYMPI_Bcast(&buffer, messagesize, MPI_CHAR, root, MPI_COMM_WORLD);

	//If not root, print the received message
	if (rank != root) {
		sender = mod((rank - 1), size);
		printf("Hi! I (task: %d) just received '%s' from task %d this message was broadcasted by %d\n", rank, buffer, sender, root);
	}

	MPI_Finalize();
}
