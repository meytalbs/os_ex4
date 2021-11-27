
//Fills the array

// ---------------------------------includes-----------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h> // sleep()

// -----------------------------structs and define-----------------------------
#define MAX_MSG_LEN 200
#define ARR_SIZE 2//10000
#define NUM_OF_GEN 3

struct Data {
	pid_t _id;
	int _prime;
};

struct my_msgbuf {
	long _mtype;
	struct Data _data;
};

int end = 0;

// ----------------------------------------------------------------------------
void get_external_id(key_t* key);
void create_queue(int* queue_id, key_t key);
void fill_array(int arr[], int queue_id, long int allowed_type);
void remove_queue(int queue_id);
int count_appear(int arr[], int num, int index, int* count_new);
void update_max_and_min(int prime, int* max_prime, int* min_prime);
void get_prime_from_queue(int queue_id, struct my_msgbuf* my_msg, long int allowed_type);
void return_appear(struct my_msgbuf* my_msg, int queue_id);
void end_fill();
// ----------------------------------------------------------------------------

int main()
{
	//struct my_msgbuf my_msg;
	long int allowed_type = 1;
	int queue_id;						// Internal queue ID
	key_t key;							// External queue ID
	int arr[ARR_SIZE];

	// todo - function that includes all?
	get_external_id(&key);
	create_queue(&queue_id, key);
	//prepar_fill();	// todo
	fill_array(arr, queue_id, allowed_type);
	//end_fill();		// todo
	remove_queue(queue_id);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void prepar_fill()
{
	// check if all the manufacturer connect to queue
	// send to them that they can start run
}
// ----------------------------------------------------------------------------

void fill_array(int arr[], int queue_id, long int allowed_type)
{
	int i,
		count_new = 0,
		max_prime = 0,
		min_prime = 0;
	struct my_msgbuf my_msg;

	for (i = 0; i < ARR_SIZE && !end; i++) 
	{
		// read num from queue
		//get_prime_from_queue(queue_id, &my_msg, allowed_type); //todo - insert msrcv to this function		
		if (msgrcv(queue_id,
			&my_msg,
			sizeof(struct Data),
			1,
			0) == -1)
		{
			perror("msgrcv failed");
			end = 1;
		}
		arr[i] = my_msg._data._prime;
		update_max_and_min(my_msg._data._prime, &max_prime, &min_prime);
		// read to queue - return appearence:
		//return_appear(&my_msg, queue_id);		
		my_msg._mtype = my_msg._data._id;
		my_msg._data._prime = count_appear(arr, my_msg._data._prime, i, &count_new);
		my_msg._data._id = getpid();
		if (!end && msgsnd(queue_id, &my_msg,
			sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			end = 1;
		}

		printf("sent %d to queue %d \n", my_msg._data._prime, my_msg._data._id);   // todo - delete
	}

	if (msgrcv(queue_id,
		&my_msg,
		sizeof(struct Data),
		1,
		0) == -1)
	{
		perror("msgrcv failed");
		end = 1;
	}

	// send to end
	my_msg._mtype = my_msg._data._id;
	my_msg._data._prime = -1;
	my_msg._data._id = getpid();
	if (msgsnd(queue_id, &my_msg,
		sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		end = 1;
	}

	printf("New prime in arr: %d\n", count_new);
	printf("Max prime in arr: %d\n", max_prime);
	printf("Min prime in arr: %d\n", min_prime);
}
// ----------------------------------------------------------------------------

void end_fill()
{
	// send to manufacturers to end
	// print num of diffrent num in the array
	// print the smaller value
	// print the max value

}
// ----------------------------------------------------------------------------

void get_prime_from_queue(int queue_id, struct my_msgbuf* my_msg, long int allowed_type)
{
	if (msgrcv(queue_id,
		&my_msg,
		sizeof(struct Data),
		1,
		0) == -1)
	{
		perror("msgrcv failed");
		printf("fail here1");
		end = 1; 
	}
}
// ----------------------------------------------------------------------------

void return_appear(struct my_msgbuf* my_msg, int queue_id)
{
	my_msg->_mtype = my_msg->_data._id;
	my_msg->_data._prime = 0; // todo //count_appear(arr, my_msg._msg_data._prime, i);
	if (!end && msgsnd(queue_id, &my_msg,
		sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		end = 1;
	}
}
// ----------------------------------------------------------------------------

// this function update max prime in array
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

	return counter;
}
// ----------------------------------------------------------------------------

// get an id for the queue
void get_external_id(key_t* key)
{
	if (((*key) = ftok(".", '6')) == -1)
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