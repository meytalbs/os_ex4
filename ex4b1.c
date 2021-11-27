
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
int count_appear(int arr[], int num, int index);
void update_min(int* min_prime, int prime);
void update_min(int* min_prime, int prime);
void get_prime_from_queue(struct my_msgbuf* my_msg);
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
		max_prime = 0,
		min_prime = 0;

	for (i = 0; i < ARR_SIZE; i++) {
		struct my_msgbuf my_msg;
		int status;
		
		// read num from queue
		//get_prime_from_queue(&my_msg); todo - insert msrcv to this function
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

		printf("Got %d from queue\n", my_msg._msg_data._prime);   // todo - delete
		//count_appear(arr, my_msg._msg_data._prime, i);
		//return_appear(); // todo
		//update_max(&max_prime, my_msg._msg_data._prime);
		//update_min(&min_prime, my_msg._msg_data._prime);
	}

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

void get_prime_from_queue(struct my_msgbuf* my_msg)
{
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
}
// ----------------------------------------------------------------------------

// this function update max prime in array
void update_max(int* max_prime, int prime)
{
	if ((*max_prime) == 0 || prime > (*max_prime))
		(*max_prime) = prime;
}
// ----------------------------------------------------------------------------

// this function update min prime in array
void update_min(int* min_prime, int prime)
{
	if ((*min_prime) == 0 || prime > (*min_prime))
		(*min_prime) = prime;
}
// ----------------------------------------------------------------------------

// this function count appearance of num in array and returns the result
int count_appear(int arr[], int num, int index)
{
    int i,
        counter = 0;

    for (i = 0; i < index; ++i)
    {
        if (arr[i] == num)
            ++counter;
    }

    return counter;
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