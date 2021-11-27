// Manufacturer numbers

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
#include <time.h>

// -----------------------------structs and define-----------------------------
#define MAX_MSG_LEN 200

struct Data {
	pid_t _id;
	int _prime;
};

struct my_msgbuf {
	long _mtype;
	struct Data _data;
};

// ----------------------------------------------------------------------------
void get_external_id(key_t* key);
void open_existing_queue(int* queue_id, key_t key);
void write_to_queue(int queue_id);
int get_prime();
int is_prime(int number);
void end_create_prime(int num_of_new_primes);
// ----------------------------------------------------------------------------

int main(void)
{
	int queue_id;						// Internal queue ID
	key_t key;							// External queue ID

	get_external_id(&key);
	open_existing_queue(&queue_id, key);
	write_to_queue(queue_id);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void write_to_queue(int queue_id)
{
	struct my_msgbuf my_msg;
	int num_of_new_primes = 0; 

	while (1)
	{
		my_msg._mtype = 1;
		my_msg._data._id = getpid();
		my_msg._data._prime = get_prime();
		if (msgsnd(queue_id,
			(struct msgbuf*)&my_msg,
			sizeof(struct Data),
			0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
		printf("%d send %d to server\n", (int)getpid(), my_msg._data._prime);//todo - delete

		// read from queue apperance of num in array
		if (msgrcv(queue_id,
			&my_msg,
			sizeof(struct Data),
			(int)getpid(),
			0) == -1)
		{
			perror("msgrcv failed");
			exit(EXIT_FAILURE);
		}

		if (my_msg._data._prime == 0)
			++num_of_new_primes;

		if (my_msg._data._prime == -1)
			break;

		printf("%d get %d from server\n", (int)getpid(), my_msg._data._prime); //todo - delete
	}
	end_create_prime(num_of_new_primes);
}
// ----------------------------------------------------------------------------

void end_create_prime(int num_of_new_primes)
{
	//print num of diff
	printf("Num of new primes %d\n: ", num_of_new_primes);
	//print value that fill get the most
	exit(EXIT_SUCCESS);
}
// ----------------------------------------------------------------------------

// this function get prime num
int get_prime()
{
	int num;
	while (!is_prime(num = rand() % 999 + 2)) {}

	return num;
}
// ----------------------------------------------------------------------------

// this function gets number and return 1 if its a prime number and 0 if not
int is_prime(int number)
{
	int i;

	if (number < 2)
		return 0;

	for (i = 2; i * i < number; ++i)
	{
		if (number % i == 0)
			return 0;
	}
	return 1;
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
