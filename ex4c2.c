
/* Application server
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

struct Data {					// for data of msg
	pid_t _id;					// prosses id
	int _action;				// prime number
	int _num;					// prime number	/ sizeof(str)
	char* _str;					// to check pelindrome
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

// ----------------------------------------------------------------------------
void signal_handler();
void create_queue(int* queue_id, key_t *key);
void remove_queue(int queue_id);
// ----------------------------------------------------------------------------

int main()
{
	int queue_id;						// Internal queue ID
	key_t key;							// External queue ID

	signal(signal_handler, SIGTINT);

	create_queue(&queue_id, &key);
	application_server();

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void application_server()
{
	struct my_msgbuf my_msg;		// for msg to send / get

	while (1)
	{
		if (msgrcv(queue_id, &my_msg, sizeof(struct Data), ALLOWED_TYPE,
																	0) == -1)
		{
			perror("msgrcv failed");
			remove_queue(queue_id);
		}

		if (is_on_db()) // todo - check if is on db
		{
			if (1 == my_msg._data._action)
				is_prime(my_msg._data._num);
			else
				is_poly(my_msg._data._str, my_msg._data._num);

		}
	}
}
// ----------------------------------------------------------------------------

// create the queue, if it exists then fail, prmssins = 0600
void create_queue(int* queue_id, key_t *key)
{
	if (((*key) = ftok(".", 'd')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	if (((*queue_id) = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------

//------------------------------------------------------------------------------
void is_prime(int number)
{
	int i, prime;

	if (number < 2)
		return 0;

	for (i = 2; i * i <= number; ++i)
	{
		if (number % i == 0)
		{
			prime = 0;
			break;
		}
	}
	prime = 1;

	if (msgsnd(queue_id, &my_msg, sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		kill(getpid(), SIGTINT);
	}
}

void is_poly(my_msg._data._str, my_msg._data._num)

//-----------------------------------------------------------------------------
// remove the queue
// at this point receiver fails to read from queue 
// (msgrcv fail) so it exits
void signal_handler()
{
	if (msgctl(queue_id, IPC_RMID, NULL) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}
}

