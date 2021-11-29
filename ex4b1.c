
/* Fills the array - ex4b1.c
==============================================================================
Written by : Meytal Abrahamian  login : meytalben  id : 211369939
			 Tomer Akrish               tomerak         315224295
==============================================================================

This program defines an array of int with SIZE_ARR cells.
Its purpose is to fulfill the the array in prime numbers.

Before the filler begins to fill the array, it waits to receive a message from
all the NUM_OF_GEN manufacturers that they are connected to the queue of 
messages. Once everyone is logged in he sends a message that you can start and
start.

The program, in each round in its main loop, reads a number
from the message queue. He checks if the number already exists in the array, 
and sends it to the manufacturer who sent him the number, how many times the
number has appeare in the array (for new number - zero)

When the filler finishes filling the array it sends the manufacturers a 
message to finish and shows how many different values there are in its array,
what the minimum value, and what is the maximum value
*/
// ---------------------------------includes-----------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h> 

// -----------------------------structs and define-----------------------------
#define ARR_SIZE 10000			// for size of main arr 
#define NUM_OF_GEN 3			// for number of gen 
#define ALLOWED_TYPE 1			// allowed type of msg of this program

struct Data {					// for data of msg
	pid_t _id;					// prosses id
	int _prime;					// prime number
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

int end = 0;					// to not end without remove queue
int gen_id[NUM_OF_GEN];			// for GEN ids

// ----------------------------------------------------------------------------
void get_external_id(key_t* key);
void create_queue(int* queue_id, key_t key);
void fill_array(int queue_id);
void remove_queue(int queue_id);
int count_appear(int arr[], int num, int index, int* count_new);
void update_max_and_min(int prime, int* max_prime, int* min_prime);
void end_fill(int queue_id, int count_new, int max_prime, int min_prime);
void prepar_fill(int queue_id);
// ----------------------------------------------------------------------------

int main()
{
	int queue_id;						// Internal queue ID
	key_t key;							// External queue ID

	get_external_id(&key);
	create_queue(&queue_id, key);
	prepar_fill(queue_id);
	fill_array(queue_id);
	remove_queue(queue_id);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

// this function get msg from all manufacturers that connected to queue and 
// after it get NUM_OF_GEN msg - it send to all NUM_OF_GEN manufacturers 
// msg to start
void prepar_fill(int queue_id)
{
	int i;							// for loop
	struct my_msgbuf my_msg;		// for msg to send / get

	// get msg from manufacturers
	for (i = 0; i < NUM_OF_GEN; ++i)
	{
		if (msgrcv(queue_id,
			&my_msg,
			sizeof(struct Data),
			ALLOWED_TYPE,
			0) == -1)
		{
			perror("msgrcv failed");
			remove_queue(queue_id);
		}
		gen_id[i] = my_msg._data._id;
	}

	// send to all manufacturers to start
	for (i = 0; i < NUM_OF_GEN; ++i)
	{
		my_msg._mtype = gen_id[i];

		my_msg._data._prime = 1;
		my_msg._data._id = getpid();

		if (msgsnd(queue_id, &my_msg,
			sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			remove_queue(queue_id);
		}
	}
}
// ----------------------------------------------------------------------------

void fill_array(int queue_id)
{
	int arr[ARR_SIZE],					// main arr of program
		i,								// for loop
		count_new = 0,					// count new prime value in arr
		max_prime = 0,					// for max prime in arr
		min_prime = 0;					// for min prime in arr
	struct my_msgbuf my_msg;			// for msg to sent or get

	for (i = 0; i < ARR_SIZE && !end; i++) 
	{
		// read prime num from queue
		if (msgrcv(queue_id,
			&my_msg,
			sizeof(struct Data),
			ALLOWED_TYPE,
			0) == -1)
		{
			perror("msgrcv failed");
			end = 1;
		}

		// insert to arr and updates
		arr[i] = my_msg._data._prime;
		update_max_and_min(my_msg._data._prime, &max_prime, &min_prime);

		// write to queue appearence of this prime
		my_msg._mtype = my_msg._data._id;
		my_msg._data._prime = count_appear(arr, my_msg._data._prime, 
											i, &count_new);
		my_msg._data._id = getpid();
		if (!end && msgsnd(queue_id, &my_msg,
			sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			end = 1;
		}
	}

	// when arr if full
	end_fill(queue_id, count_new, max_prime, min_prime);
}
// ----------------------------------------------------------------------------

// this function do the things that need to do after fill the arr
// send to manufacturers they need to end and prints data
void end_fill(int queue_id, int count_new, int max_prime, int min_prime)
{
	struct my_msgbuf my_msg;		// for msg to send / get
	int i;							// for loops

	// get the last msg in queue
	if (msgrcv(queue_id,			
		&my_msg,
		sizeof(struct Data),
		ALLOWED_TYPE,
		0) == -1)
	{
		perror("msgrcv failed");
		remove_queue(queue_id);
	}
	// send to all manufacturers to end
	for (i = 0; i < NUM_OF_GEN; ++i)
	{
		my_msg._mtype = gen_id[i];
		my_msg._data._prime = -1;
		my_msg._data._id = getpid();
		if (msgsnd(queue_id, &my_msg,
			sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			remove_queue(queue_id);
		}
	}
	// get from all manufacturers msg they end
	for (i = 0; i < NUM_OF_GEN; ++i)
	{
		if (msgrcv(queue_id,
			&my_msg,
			sizeof(struct Data),
			ALLOWED_TYPE,
			0) == -1)
		{
			perror("msgrcv failed");
			remove_queue(queue_id);
		}
	}

	// print data
	printf("New prime in arr: %d\n", count_new);
	printf("Max prime in arr: %d\n", max_prime);
	printf("Min prime in arr: %d\n", min_prime);
}
// ----------------------------------------------------------------------------

// this function update max prime and min prime in array
void update_max_and_min(int prime, int* max_prime, int* min_prime)
{
	if ((*max_prime) == 0 || prime > (*max_prime))
		(*max_prime) = prime;

	if ((*min_prime) == 0 || prime < (*min_prime))
		(*min_prime) = prime;
}
// ----------------------------------------------------------------------------

// this function count appearance of num in array and returns the result
int count_appear(int arr[], int num, int index, int* count_new)
{
	int i,
		counter = 0;

	for (i = 0; i < index; ++i)
	{
		if (arr[i] == num)
			++counter;
	}

	if (counter == 0)
		++(*count_new);

	return counter+1;
}
// ----------------------------------------------------------------------------

// get an id for the queue
void get_external_id(key_t* key)
{
	if (((*key) = ftok(".", '4')) == -1)
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