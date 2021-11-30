
/* Client
==============================================================================
Written by : Meytal Abrahamian  login : meytalben  id : 211369939
			 Tomer Akrish               tomerak         315224295
==============================================================================

This program is for a client that use 2 servers in ex4c1 and ex4c2.
The client connects to the two message queues:
the registration server and the applications server and registered with the
registry server.

In the loop:
1. The customer reads from the user the notes indicating whether the user
wants to handle the number or in a string ('n' - number, 's' - string)
Then the customer reads the required figure.

2. The client sends the appropriate message to the application server, waits
for a response, and displays it on the screen.

3. When the customer reads the character 'e', he sends a message to the
registry server that it must be removed from the system, and he finishes.
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

// ---------------------------------defines------------------------------------
#define REGISTER 1				// 1 indicates a connection to the server
#define ALLOWED_TYPE 1			// allowed type of msg of this program
#define EXIT_PROCESS 3			// 1 indicates delete from server
#define reg_server 1
#define check_prime 1
#define allowed_type 3
#define check_pali 2
#define server_app 2
#define REG_CHAR 'c'			// char to registr queue id
#define APP_CHAR 'd'			// char to application queue id
#define TYPE_NUMBER 'n'			// for user type 'n'
#define TYPE_STRING 's'			// for user type 's'
#define TYPE_EXIT 'e'			// for user type 'e'

// ----------------------------------structs-----------------------------------
struct Data_app {					// for data of msg
	pid_t _id;					// prosses id
	int _action;				// action to do
	int _num;					// prime number	/ sizeof(str)
	char* _str;
};

struct msgbuf_app {				// struct for msg
	long _mtype;				// for msg type
	struct Data_app _data;			// for msg data
};

struct Data_reg {				// for data of msg
	pid_t _id;					// prosses id
	int _action;				// action
};

struct msgbuf_reg {				// struct for msg
	long _mtype;				// for msg type
	struct Data_reg _data;		// for msg data
};

// ----------------------------------------------------------------------------
void connect_to_queue(key_t* key, int* qid, char _char);
void registr_to_server(int reg_id);
void client(int app_qid, int reg_qid);
void num_handler(int app_qid);
void string_handler(int app_qid);
void exit_handler(int reg_qid);
void send_msg_app_server(struct msgbuf_app* my_msg, int qid);
void exit_with_msg(char* string);
// ----------------------------------------------------------------------------

// --------------------------------main----------------------------------------
int main()
{
	key_t reg_key, app_key;		// registration/application queue key
	int reg_qid, app_qid;		// registration/application queue id

	connect_to_queue(&reg_key, &reg_qid, REG_CHAR);
	registr_to_server(reg_qid);

	connect_to_queue(&app_key, &app_qid, APP_CHAR);

	client(app_qid, reg_qid);

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

// this function run client active
void client(int app_qid, int reg_qid)
{
	char active;		// input from user

	printf("Enter 'n' to check if a number is prime,\n");
	printf("'s' to check if a string is a palindrome\nor 'e' to exit\n");

	while (1)
	{
		scanf("%c", &active);

		if (active == TYPE_NUMBER)
			num_handler(app_qid);
		else if (active == TYPE_STRING)
			string_handler(app_qid);
		else if (active == TYPE_EXIT)
			exit_handler(reg_qid);
	}
}
// ----------------------------------------------------------------------------

// this function registr the program prosses to registration server
void registr_to_server(int reg_qid)
{
	struct msgbuf_reg msg_reg;		// for msg to send / get from queue

	msg_reg._mtype = reg_server;
	msg_reg._data._action = REGISTER;
	msg_reg._data._id = getpid();

	if (msgsnd(reg_qid, &msg_reg, sizeof(struct Data_reg), 0) == -1)
		exit_with_msg("msgsnd failed");

	// get response from server
	if (msgrcv(reg_qid, &msg_reg, sizeof(struct Data_reg), allowed_type, 0) == -1)
		exit_with_msg("msgrcv failed");

	// check if process was registred successfully		//reg to serever/full
	if (msg_reg._data._action == 1 || msg_reg._data._action == 2)
		exit_with_msg("cannot register to server\n");
}
// ----------------------------------------------------------------------------

// this function connected to some queue
void connect_to_queue(key_t* key, int* qid, char _char)
{
	if (((*key) = ftok(".", _char)) == -1)
		exit_with_msg("ftok failed");
	if (((*qid) = msgget((*key), 0)) == -1)
		exit_with_msg("msgget failed");
}
// ----------------------------------------------------------------------------

// input was 'n', check if a number is prime
void num_handler(int app_qid)
{
	struct msgbuf_app msg_app;		// for msg to send / get from queue

	msg_app._mtype = server_app;
	scanf("%d", &msg_app._data._num);
	msg_app._data._id = getpid();
	msg_app._data._action = check_prime;

	send_msg_app_server(&msg_app, app_qid);

	if (msgrcv(app_qid, &msg_app, sizeof(struct Data_app), 3, 0) == -1)
		exit_with_msg("error with msgrcv");

	printf("is the number prime?: %d\n", msg_app._data._num);
}
// ----------------------------------------------------------------------------

// input was 's', check if a string is palindrome 
void string_handler(int app_qid)
{
	struct msgbuf_app msg_app;		// for msg to send / get from queue
	char* str = NULL;
	scanf("%s", str);

	msg_app._mtype = server_app;
	msg_app._data._id = getpid();
	msg_app._data._str = str;
	puts("mirmir");
	printf("in here, strlen = :%s", (str));
	msg_app._data._num = (int)strlen(str);
	msg_app._data._action = check_pali;
	send_msg_app_server(&msg_app, app_qid);

	if (msgrcv(app_qid, &msg_app, sizeof(struct Data_app), allowed_type, 0) == -1)
		exit_with_msg("error with msgrcv");

	printf("is the string a palindrome?: %d\n", msg_app._data._num);
}
// ----------------------------------------------------------------------------

// input was 'e', end program (and delete process from server)
void exit_handler(int reg_qid)
{
	// for msg to send / get from queue
	struct msgbuf_reg msg_reg = { reg_server, {getpid(), EXIT_PROCESS} };
	/*
	msg_reg._data._action = EXIT_PROCESS;
	msg_reg._data._id = getpid();
	msg_reg._mtype = reg_server;
	*/
	if (msgsnd(reg_qid, &msg_reg, sizeof(struct Data_reg), 0) == -1)
		exit_with_msg("msgsnd failed");

	//sleep(10);
	exit(EXIT_SUCCESS);
}
// ----------------------------------------------------------------------------

// this function is a general function to send msg to server
void send_msg_app_server(struct msgbuf_app* my_msg, int qid)
{
	if (msgsnd(qid, my_msg, sizeof(struct Data_app), 0) == -1)
		exit_with_msg("msgsnd failed");
}
// ----------------------------------------------------------------------------

void exit_with_msg(char* string)
{
	perror(string);
	exit(EXIT_FAILURE);
}
// ----------------------------------------------------------------------------
