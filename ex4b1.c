
//Fills the array

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
#define ARR_SIZE 10000
#define NUM_OF_GEN 3

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
void create_queue(int* queue_id, key_t key);
void fill_array(int arr[], int queue_id, long int allowed_type);
void remove_queue(int queue_id);
// ----------------------------------------------------------------------------

int main()
{
	//struct my_msgbuf my_msg;
	long int allowed_type = 1;
	int queue_id;						// Internal queue ID
	key_t key;						// External queue ID
	int arr[ARR_SIZE];

	get_external_id(&key);
	create_queue(&queue_id, key);

	// check if all the manufacturer connect to queue
	// send to them that they can start run

	fill_array(arr, queue_id, allowed_type);

	// send to manufacturers to end

	// print num of diffrent num in the array
	// print the smaller value
	// print the max value

	remove_queue(queue_id);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void fill_array(int arr[], int queue_id, long int allowed_type)
{
	int i;
	for (i = 0; i < 2; i++) {
		struct my_msgbuf my_msg;
		int status;

		status = msgrcv(queue_id,
			(struct msgbuf*)&my_msg,
			sizeof(struct data),
			allowed_type,
			0);
		if (status == -1)
		{
			perror("msgrcv failed");
			exit(EXIT_FAILURE);
		}
		printf("Got %d from queue\n", my_msg._msg_data._prime);
	}
	// read num from queue to fill the array
	// for (i = 0; i < ARR_SIZE; ++i)
	// {
	// 	// read num from queue
	// 	// check if is in array
	// 	// writh to manufacturer apperaence of num in array
	// 	// update max num
	// 	// update min num

	// 	struct my_msgbuf my_msg;
	// 	int status;
	// 	status = msgrcv(queue_id,
	// 					(struct msgbuf*)&my_msg,
	// 					MAX_MSG_LEN,
	// 					allowed_type,
	// 					0);

	// 	if (status == -1) 
	// 	{
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

// create the queue, if it exists then fail, prmssins = 0600
void create_queue(int* queue_id, key_t key)
{
	if (((*queue_id) = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------

// remove the queue
// at this point receiver fails to read from queue 
// (msgrcv fail) so it exits
void remove_queue(int queue_id)
{
	if (msgctl(queue_id, IPC_RMID, NULL) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------