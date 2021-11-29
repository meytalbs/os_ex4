
/* Client
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
	int _num;					// prime number
};

struct my_msgbuf {				// struct for msg
	long _mtype;				// for msg type
	struct Data _data;			// for msg data
};

// ----------------------------------------------------------------------------
void client();
// ----------------------------------------------------------------------------

int main()
{
	client();

	return EXIT_SUCCESS;
}
// ----------------------------------------------------------------------------

void client()
{
	struct my_msgbuf my_msg;
	int reg_qid,
		app_qid;
	key_t reg_key,
		  app_key;
	char active;

	//connect_to_queues();
	if ((reg_key = ftok(".", 'c')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((reg_qid = msgget(key, 0)) == -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
	if ((app_key = ftok(".", 'd')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((app_qid = msgget(key, 0)) == -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
	// registr on registration server
	my_msg.mtype = 1;
	my_msg._data._id = getpid();
	my_msg._data._num = 1;  // add prosses
	printf("%d send %d to server\n", (int)getpid(), my_msg._data._num);

	if (msgsnd(reg_qid, &my_msg,
		sizeof(struct Data), 0) == -1) 
	{
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	}
	// 
	if (msgrcv(reg_qid, &my_msg,
		sizeof(struct Data), (int)getpid(), 0) == -1) 
	{
		perror("msgrcv failed");
		exit(EXIT_FAILURE);
	}
	// if cant register end faliur

	while (scanf(" %c", &active) != EOF)
	{
		switch (active)
		{
		n:
			scanf(" %d", &my_msg._data._num);
			// number
			my_msg.mtype = 1;
			my_msg._data._id = getpid();
			//my_msg._data._num = 1;  // add prosses
			printf("%d send %d to server\n", (int)getpid(), my_msg._data._num);

			if (msgsnd(app_qid, &my_msg,
				sizeof(struct Data), 0) == -1)
			{
				perror("msgsnd failed");
				exit(EXIT_FAILURE);
			}
			break;

		s: // string
			scanf(" %s", &);
			my_msg.mtype = 1;
			my_msg._data._id = getpid();
			//my_msg._data._num = 1;  // add prosses
			printf("%d send %d to server\n", (int)getpid(), my_msg._data._num);

			if (msgsnd(app_qid, &my_msg,
				sizeof(struct Data), 0) == -1)
			{
				perror("msgsnd failed");
				exit(EXIT_FAILURE);
			}
			break;
		e: // end
			my_msg.mtype = 1;
			my_msg._data._id = getpid();
			//my_msg._data._num = 1;  // add prosses
			printf("%d send %d to server\n", (int)getpid(), my_msg._data._num);

			if (msgsnd(app_qid, &my_msg,
				sizeof(struct Data), 0) == -1)
			{
				perror("msgsnd failed");
				exit(EXIT_FAILURE);
			}
			sleep(10);
			exit(EXIT_SUCCESS);
			break;

		default:
			break;
		}
	}


}