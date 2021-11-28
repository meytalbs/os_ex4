
/* Registration server
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


#define DB_SIZE 100

int queue_id;
int value_in_db = 0;

struct Data {					// for data of msg
	pid_t _id;					// prosses id
	int _num;					// prime number
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

// ----------------------------------------------------------------------------
void signal_handler();
void registration_server();
void get_external_id(key_t* key);
void create_queue(key_t key);
void remove_queue();
void delete_prosses(int db[], int id);
void is_on_db(int db[], int id);
void add_prosses(int db[], int id);
void add_to_db(int db[], int id);
int find_id_index(int db[], int id);
// ----------------------------------------------------------------------------

int main()
{
	signal(signal_handler, SIGTINT);

	registration_server();

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void registration_server()
{
	int db[DB_SIZE];
	key_t key;							// External queue ID
	struct my_msgbuf my_msg;		// for msg to send / get

	create_queue(key);

	while (1)
	{
		if (msgrcv(queue_id, &my_msg, sizeof(struct Data),
				   ALLOWED_TYPE, 0) == -1)
		{
			perror("msgrcv failed");
			remove_queue(queue_id);
		}
		
		switch (my_msg._data._num)
		{
		case: ADD_PROSSES
			add_prosses(db, my_msg._data._id);
			break;
		case: IS_ON_DB
			is_on_db(db, my_msg._data._id);
			break;
		case: DELETE_PROSSES
			delete_prosses(db, my_msg._data._id);
			break;
		default:
			break;
		}	
	}
}
// ----------------------------------------------------------------------------

void delete_prosses(int db[], int id)
{
	int index = find_id_index(db, id);
	int i;

	for (i = index; i < (value_in_db - index); ++i)
		db[i] = db[i + 1];

	--value_in_db;
}
// ----------------------------------------------------------------------------

void is_on_db(int db[], int id)
{
	int index = find_id_index(db, id);

	if (index == -1)
		printf("%d\n", 0);
	else
		printf("%d\n", 1);
}
// ----------------------------------------------------------------------------

void add_prosses(int db[], int id)
{
	int index = find_id_index(db, id);
	struct my_msgbuf my_msg;		// for msg to send / get

	if (index == -1)
	{
		add_to_db(db, id);
		my_msg._data._num = 0;		// prosses added
	}
	else if (value_in_db == DB_SIZE)
		my_msg._data._num = 2;		// db id full
	else
		my_msg._data._num = 1;		// is in db

	my_msg._mtype = id;
	my_msg._data._id = getpid();

	if (msgsnd(queue_id, &my_msg,
		sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		remove_queue(queue_id);
	}
}
// ----------------------------------------------------------------------------

void add_to_db(int db[], int id)
{
	db[value_in_db] = id;
	--value_in_db;
}
// ----------------------------------------------------------------------------

int find_id_index(int db[], int id)
{
	int i;

	for (i = 0; i < DB_SIZE; ++i)
	{
		if (db[i] == id)
			return i;
	}

	return -1
}
// ----------------------------------------------------------------------------

// get an id for the queue
void get_external_id(key_t* key)
{
	if (((*key) = ftok(".", 'c')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------

// create the queue, if it exists then fail, prmssins = 0600
void create_queue(key_t key)
{
	get_external_id(&key);

	if ((queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------

void signal_handler()
{
	remove_queue();
}
// ----------------------------------------------------------------------------

// remove the queue
// at this point receiver fails to read from queue 
// (msgrcv fail) so it exits
void remove_queue()
{
	if (msgctl(queue_id, IPC_RMID, NULL) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------