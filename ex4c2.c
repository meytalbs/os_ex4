
/* Application server
==============================================================================
Written by : Meytal Abrahamian  login : meytalben  id : 211369939
			 Tomer Akrish               tomerak         315224295
==============================================================================

This program is a server that allows those who are already logged in to 
perform actions: (A) Check initiality, (b) Check plinths. 
The server generates a queue of messages.

The server allows two operations:
1. Checking the primacy of a number.
2. Checking the plinthromeness of a string

When he receives a message requesting action, he checks to see if the applicant
is known to the registry server. If so, he performs the operation and returns 
an answer (0 or 1,) if the applicant is not registered in the system it returns
the value 1.
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

//--------------------------globals n structs-----------------------------------
#define client 3
#define reg_server 1
#define check_prime 1
#define allowed_type 2
#define in_db 2
#define string_size 100

struct Data_app {
	pid_t _id;
	int _action;
	int _num;					// prime number	/ sizeof(str)
	char _str[string_size];		// to check pelindrome
};

struct msgbuf_app {
	long _mtype;				// app msg type
	struct Data_app _data;		// app msg data
};

struct Data_reg {
	pid_t _id;
	int _action;
};

struct msgbuf_reg {
	long _mtype;				// reg msg type
	struct Data_reg _data;		// reg msg data
};

int queue_id_reg, queue_id_app;

// ----------------------------------------------------------------------------
void signal_handler(int sig_num);
void connect_to_queue(key_t* key);
void create_queue(key_t* key);
void application_server();
int is_on_db(struct msgbuf_app msg_app);
int is_prime(int number);
void prime_handler(int number);
void pali_handler(char* str, int size);
int is_poly(char* str, int size);

// ----------------------------------------------------------------------------
int main()
{
	key_t key_reg, key_app;							// External queue ID

	signal(SIGINT, signal_handler);

	connect_to_queue(&key_reg);
	create_queue(&key_app);
	application_server();

	return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------

void connect_to_queue(key_t* key)
{
	if (((*key) = ftok(".", 'c')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((queue_id_reg = msgget((*key), 0)) == -1)
	{
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
}
//-----------------------------------------------------------------------------

void create_queue(key_t* key)
{
	if (((*key) = ftok(".", 'd')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	if ((queue_id_app = msgget(*key, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------

void application_server()
{
	struct msgbuf_app msg_app;		// for msg to send / get

	while (1)
	{
		if (msgrcv(queue_id_app, &msg_app, sizeof(struct Data_app), allowed_type, 0) == -1)
		{
			perror("msgrcv failed");
			kill(getpid(), SIGINT);
		}

		if (is_on_db(msg_app))
		{
			if (check_prime == msg_app._data._action)
				prime_handler(msg_app._data._num);
			else
				pali_handler(msg_app._data._str, msg_app._data._num);
		}
	}
}
// ----------------------------------------------------------------------------

int is_on_db(struct msgbuf_app msg_app)
{
	struct msgbuf_reg msg_reg = { reg_server, {msg_app._data._id, in_db} };

	if (msgsnd(queue_id_reg, &msg_reg, sizeof(struct Data_reg), 0) == -1)
	{
		perror("msgsnd failed");
		kill(getpid(), SIGINT);
	}
	if (msgrcv(queue_id_reg, &msg_reg, sizeof(struct Data_reg), allowed_type, 0) == -1)
	{
		perror("msgrcv failed");
		kill(getpid(), SIGINT);
	}
	return msg_reg._data._action;//1 if in db		0 if isnt in db
}
// ----------------------------------------------------------------------------

void prime_handler(int number)
{
	struct msgbuf_app msg_app;
	msg_app._data._num = is_prime(number);
	msg_app._mtype = client;

	if (msgsnd(queue_id_app, &msg_app, sizeof(struct Data_app), 0) == -1)
	{
		perror("msgsnd failed");
		kill(getpid(), SIGINT);
	}
}

//------------------------------------------------------------------------------
int is_prime(int number)
{
	int i;

	if (number < 2)
		return 0;

	for (i = 2; i * i <= number; ++i)
	{
		if (number % i == 0)
			return 0;
	}
	return 1;
}

//------------------------------------------------------------------------------
void pali_handler(char* str, int size)
{
	struct msgbuf_app msg_app;

	msg_app._data._num = is_poly(str, size);
	msg_app._mtype = client;

	if (msgsnd(queue_id_app, &msg_app, sizeof(struct Data_app), 0) == -1)
	{
		perror("msgsnd failed");
		kill(getpid(), SIGINT);
	}
}

//------------------------------------------------------------------------------
int is_poly(char* str, int size)
{
	printf("%d\n", size);
	int start = 0, end = size - 1;
	while (start < end)
	{
		if (str[start++] != str[end--])
			return 0;
	}
	return 1;
}

//-----------------------------------------------------------------------------
void signal_handler(int sig_num)
{
	if (msgctl(queue_id_app, IPC_RMID, NULL) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
