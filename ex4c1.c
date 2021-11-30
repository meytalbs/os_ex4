
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

//--------------------------globals n structs-----------------------------------
#define DB_SIZE 100
#define not_found -1
#define server_app 2
#define client 3
#define added 0
#define in_db 1
#define db_full 2
#define allowed_type 1

int queue_id;
int value_in_db = 0;

struct Data {					// for data of msg
	pid_t _id;					// prosses id
	int _action;
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

enum action { ADD_PROCESS = 1, IS_ON_DB, DELETE_PROSSES };

//-----------------------------------------------------------------------------
void signal_handler(int sig_num);
void registration_server();
void create_queue(key_t* key);
void add_process(pid_t db[], pid_t id);
void add_to_db(pid_t db[], pid_t id);
void check_on_db(pid_t db[], pid_t id);
void delete_process(pid_t db[], pid_t id);
int is_on_db(pid_t db[], pid_t id);
int find_id_index(pid_t db[], pid_t id);

// ----------------------------------------------------------------------------

int main()
{
	key_t key;							// External queue ID
	signal(SIGINT, signal_handler);

	create_queue(&key);
	registration_server();

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void registration_server()
{
	pid_t db[DB_SIZE] = { 0 };

	struct my_msgbuf my_msg;			// for msg to send / get

	while (1)
	{
		if (msgrcv(queue_id, &my_msg, sizeof(struct Data), allowed_type, 0) == -1)
		{
			perror("msgrcv failed");
			kill(getpid(), SIGINT);
		}

		switch (my_msg._data._action)
		{
		case ADD_PROCESS:
			add_process(db, my_msg._data._id);
			break;
		case IS_ON_DB:
			check_on_db(db, my_msg._data._id);
			break;
		case DELETE_PROSSES:
			delete_process(db, my_msg._data._id);
			break;
		default:
			break;
		}
	}
}
// ----------------------------------------------------------------------------

// create the queue, if it exists then fail, prmssins = 0600
void create_queue(key_t* key)
{
	if (((*key) = ftok(".", 'c')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	if ((queue_id = msgget(*key, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
}
//-----------------------------------------------------------------------------

void add_process(pid_t db[], pid_t id)
{
	struct my_msgbuf my_msg;		// for msg to send
	int found = is_on_db(db, id);
	if (!found && value_in_db < DB_SIZE)
	{
		add_to_db(db, id);
		my_msg._data._action = added;
	}
	else if (found)
		my_msg._data._action = in_db;
	else
		my_msg._data._action = db_full;

	my_msg._mtype = client;

	if (msgsnd(queue_id, &my_msg, sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		kill(getpid(), SIGINT);
	}
}
//-----------------------------------------------------------------------------

void add_to_db(pid_t db[], pid_t id)
{
	int i;
	for (i = 0; i < DB_SIZE; ++i)
	{
		if (0 == db[i])
		{
			db[value_in_db] = id;
			++value_in_db;
		}
	}
}
// ----------------------------------------------------------------------------

void check_on_db(pid_t db[], pid_t id)
{
	struct my_msgbuf my_msg;				 // for msg to send
	my_msg._mtype = server_app;
	my_msg._data._action = is_on_db(db, id); //1 if in db		0 if isnt in db

	if (msgsnd(queue_id, &my_msg, sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		kill(getpid(), SIGINT);
	}
}
//-----------------------------------------------------------------------------

void delete_process(pid_t db[], pid_t id)
{
	db[find_id_index(db, id)] = 0;//maybe add if in db
	--value_in_db;
}
//-----------------------------------------------------------------------------

int is_on_db(pid_t db[], pid_t id)
{
	return (find_id_index(db, id) == not_found ? 0 : 1);
}
// ----------------------------------------------------------------------------

int find_id_index(pid_t db[], pid_t id)
{
	int i;

	for (i = 0; i < DB_SIZE; ++i)
	{
		if (db[i] == id)
			return i;
	}

	return not_found;
}
//-----------------------------------------------------------------------------

void signal_handler(int sig_num)
{
	if (msgctl(queue_id, IPC_RMID, NULL) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}
}
// ----------------------------------------------------------------------------


