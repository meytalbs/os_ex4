
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
#define EXIT_PROSSES 3			// 1 indicates delete from server
#define REG_CHAR 'c'			// char to registr queue id
#define APP_CHAR 'd'			// char to application queue id
#define TYPE_NUMBER 'n'			// for user type 'n'
#define TYPE_STRING 's'			// for user type 's'
#define TYPE_EXIT 'e'			// for user type 'e'

// ----------------------------------structs-----------------------------------
struct Data {					// for data of msg
	pid_t _id;					// prosses id
	int _num;					// prime number
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

// ----------------------------------------------------------------------------
void client();
void registr_to_server(int reg_id);
void num_handler(int app_qid);
void string_handler(int app_qid);
void exit_handler(int reg_qid);
void connect_to_queue(key_t* key, int* qid, char _char);
void send_msg_to_server(struct my_msgbuf* my_msg, int qid);
void exit_with_msg(char* string);
// ----------------------------------------------------------------------------

// --------------------------------main----------------------------------------
int main()
{
	client();

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

// this function run client active
void client()
{
	int reg_qid,		// registration queue id
		app_qid;		// application queue id
	key_t reg_key,		// registration queue key
		  app_key;		// application queue key
	char active;		// input from user

	connect_to_queue(&reg_key, &reg_qid, REG_CHAR);
	connect_to_queue(&app_key, &app_qid, APP_CHAR);

	registr_to_server(reg_qid);	

	while (scanf(" %c", &active) != EOF)
	{
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
	struct my_msgbuf my_msg;		// for msg to send / get from queue

	my_msg._data._num = REGISTER;

	// register to registration server	
	send_msg_to_server(&my_msg, reg_qid);

	// get response from server
	if (msgrcv(reg_qid, &my_msg, sizeof(struct Data), (int)getpid(), 0) == -1)
		exit_with_msg("msgrcv failed");

	// check if process was registred successfully
	if (my_msg._data._num != 0)
		exit_with_msg("cannot register to server\n");
}
// ----------------------------------------------------------------------------

// this function handle with case that user input was 'n' which mean he 
// wants to check a number
void num_handler(int app_qid)
{
	struct my_msgbuf my_msg;		// for msg to send / get from queue

	scanf(" %d", &my_msg._data._num);	
	send_msg_to_server(&my_msg, app_qid);
}
// ----------------------------------------------------------------------------

// this function handle with case that user input was 's' which mean he 
// wants to check a string
void string_handler(int app_qid)
{
	struct my_msgbuf my_msg;		// for msg to send / get from queue
	char string;	// todo

	scanf(" %s", &string); // todo - acording to struct in c2
	send_msg_to_server(&my_msg, app_qid);
}
// ----------------------------------------------------------------------------

// this function handle with case that user input was 'e' which mean he 
// wants to end program (and delete prosses from server)
void exit_handler(int reg_qid)
{
	struct my_msgbuf my_msg;		// for msg to send / get from queue

	my_msg._data._num = EXIT_PROSSES; 
	send_msg_to_server(&my_msg, reg_qid);
	
	sleep(10);
	exit(EXIT_SUCCESS);
}
// ----------------------------------------------------------------------------

// this function is a general function to send msg to server
void send_msg_to_server(struct my_msgbuf* my_msg, int qid)
{
	my_msg->_mtype = ALLOWED_TYPE;
	my_msg->_data._id = getpid();

	if (msgsnd(qid, my_msg, sizeof(struct Data), 0) == -1)		
		exit_with_msg("msgsnd failed");
	
	printf("%d send %d to server\n", (int)getpid(), my_msg->_data._num);
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

void exit_with_msg(char* string)
{
	perror(string);
	exit(EXIT_FAILURE);
}
// ----------------------------------------------------------------------------
