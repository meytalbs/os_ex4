
/*  Manufacturer numbers - ex4b2.c
==============================================================================
Written by : Meytal Abrahamian  login : meytalben  id : 211369939
			 Tomer Akrish               tomerak         315224295
==============================================================================



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
#include <time.h>

// -----------------------------structs and define-----------------------------
struct Data {					// for data of msg
	pid_t _id;					// prosses id
	int _prime;					// prime number
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

// ----------------------------------------------------------------------------
void get_external_id(key_t* key);
void open_existing_queue(int* queue_id, key_t key);
void write_to_queue(int queue_id);
int get_prime();
int is_prime(int number);
void end_create_prime(int queue_id, int num_of_new_primes, int max_appear_prime, int max_appear);
int start(int queue_id);
// ----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int queue_id;						// Internal queue ID
	key_t key;							// External queue ID

	srand((unsigned int)(*argv[1]));
	if (argc != 2)
		return EXIT_FAILURE;

	get_external_id(&key);
	open_existing_queue(&queue_id, key);
	if (start(queue_id))
		write_to_queue(queue_id);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

// this function send msg to queue that say it connect to queue and get msg 
// from queue that it can start create primes
int start(int queue_id)
{
	struct my_msgbuf my_msg;		// for msg to send / get

	// send msg to queue
	my_msg._mtype = 1;
	my_msg._data._id = getpid();
	my_msg._data._prime = 1;
	if (msgsnd(queue_id,
		(struct msgbuf*)&my_msg,
		sizeof(struct Data),
		0) == -1)
	{
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	}

	// get msg from queue
	if (msgrcv(queue_id,
		&my_msg,
		sizeof(struct Data),
		(int)getpid(),
		0) == -1)
	{
		perror("msgrcv failed");
		exit(EXIT_FAILURE);
	}

	// return that it can start
	return 1;
}
// ----------------------------------------------------------------------------

// this function create primes and write them in queue
void write_to_queue(int queue_id)
{
	struct my_msgbuf my_msg;		// for msg to send / get
	int num_of_new_primes = 0;		// for num of new prime 
	int max_appear_prime = 0,		// for the prime that appear the most
		max_appear = 0,				// the appearence of the above prime
		temp;						// for temp

	while (1)
	{
		// write prime num to queue
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
		temp = my_msg._data._prime;

		// read from queue apperance of the prime
		if (msgrcv(queue_id,
			&my_msg,
			sizeof(struct Data),
			(int)getpid(),
			0) == -1)
		{
			perror("msgrcv failed");
			exit(EXIT_FAILURE);
		}

		// check if get msg to end
		if (my_msg._data._prime == -1)
			break;

		// update appear
		if (my_msg._data._prime > max_appear || 
			(my_msg._data._prime == 0 && max_appear == 0)) // todo
		{
			max_appear = my_msg._data._prime;
			max_appear_prime = temp;
		}

		// update counter of new primes
		if (my_msg._data._prime == 0)
			++num_of_new_primes;
	}

	// end
	end_create_prime(queue_id, num_of_new_primes, max_appear_prime, max_appear);
}
// ----------------------------------------------------------------------------

// this function end the create prime
void end_create_prime(int queue_id, int num_of_new_primes, 
					  int max_appear_prime, int max_appear)
{
	struct my_msgbuf my_msg;			// for msg to get / send

	// send msg to queue that preper to end
	my_msg._mtype = 1;
	my_msg._data._id = getpid();
	my_msg._data._prime = 1;
	if (msgsnd(queue_id,
		(struct msgbuf*)&my_msg,
		sizeof(struct Data),
		0) == -1)
	{
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	}

	// prints data
	printf("Num of new primes %d\n ", num_of_new_primes);
	printf("The prime with max appearance in arr is %d it appear %d times.\n"
		, max_appear_prime, max_appear);
}
// ----------------------------------------------------------------------------

// this function get prime num
int get_prime()
{
	int num;		// for prime num

	while (!is_prime(num = rand() % 999 + 2)) {}

	return num;
}
// ----------------------------------------------------------------------------

// this function gets number and return 1 if its a prime number and 0 if not
int is_prime(int number)
{
	int i;		    // for loop

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
	if (((*key) = ftok(".", '4')) == -1)
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
