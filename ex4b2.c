// Manufacturer numbers

// ---------------------------------includes-----------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// -----------------------------structs and define-----------------------------
#define MAX_MSG_LEN 200

struct data {
	int _manu_id;
	int _prime;
};

struct my_msgbuf {
	long _mtype;
	struct data _msg_data;
};

// ----------------------------------------------------------------------------
void get_external_id(key_t* key);
void open_existing_queue(int* queue_id, key_t key);
void write_to_queue(int queue_id);
// ----------------------------------------------------------------------------

int main(void)
{
	//long int allowed_type = 1;
	int queue_id;						// Internal queue ID
	key_t key;							// External queue ID

	get_external_id(&key);
	open_existing_queue(&queue_id, key);
	write_to_queue(queue_id);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

// end func
// print num of diff
// print value that fill get the most

void write_to_queue(int queue_id)
{
	struct my_msgbuf my_msg;

	my_msg._mtype = 1;
	while (scanf(" %d", &my_msg._msg_data._prime) != EOF)
	{
		my_msg._msg_data._manu_id = 1;
		if (msgsnd(queue_id,
			(struct msgbuf*)&my_msg,
			sizeof(struct data),
			0) == -1) {
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
		my_msg._mtype++;
		if (my_msg._mtype > 3)
			my_msg._mtype = 1;
	}

	// create num
	// while()
	// {
	// 	// get random num
	// 	// send prime to queue with id
	// 	// read from queue apperance of num in array
	// 	// update diff num

	// 	// check if need to end (in the msg it get) and if need to - call end func
	// 	struct my_msgbuf my_msg;
	// 	int status;
	// 	status = msgrcv(queue_id,
	// 		(struct msgbuf*)&my_msg,
	// 		MAX_MSG_LEN,
	// 		allowed_type,
	// 		0);
	// 	if (status == -1) {
	// 		perror("msgrcv failed");
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	printf("Got %s from queue\n", my_msg.mtext);
	// }
}
// ----------------------------------------------------------------------------

// get an id for the queue
void get_external_id(key_t* key)
{
	if (((*key) = ftok(".", '5')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------

// open the existing queue
void open_existing_queue(int* queue_id, key_t key)
{
	(*queue_id) = msgget(key, 0);
	if ((*queue_id) == -1)
	{
		perror("msgget failed in reader");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------
