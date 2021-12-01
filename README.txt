==============================================================================
Written by : Meytal Abrahamian  login : meytalben  id : 211369939
			 Tomer Akrish               tomerak         315224295
==============================================================================


Program file: ex4a1.c
===============================================================================

Program file: ex4a2.c
===============================================================================

Program file: ex4b1.c
===============================================================================
This program defines an array of int with SIZE_ARR cells.
Its purpose is to fulfill the the array in prime numbers.

Before the filler begins to fill the array, it waits to receive a message from
all the NUM_OF_GEN manufacturers that they are connected to the queue of 
messages. Once everyone is logged in he sends a message that you can start and
start.

The program, in each round in its main loop, reads a number
from the message queue. He checks if the number already exists in the array, 
and sends it to the manufacturer who sent him the number, how many times the
number has appeare in the array (for new number - zero)

When the filler finishes filling the array it sends the manufacturers a 
message to finish and shows how many different values there are in its array,
what the minimum value, and what is the maximum value

Input: no input from user only from queue (as writen above)
Output: 
		New prime in arr: SUM_OF_NEW_VALUE 
		Max prime in arr: THE_MAX_VALUE_IN_ARRAY
		Min prime in arr: THE_MIN_VALUE_IN_ARRAY

compile: gcc -Wall ex4b1.c -o ex4b1
run: ./ex3b1  

importent: must to run it befor program in ex4b2.c!


Program file: ex4b2.c
===============================================================================
This program find prime numbers and sent them in queue to array filler.

At the beginning of the program (before the manufacturer starts looking for 
prime numbers) he sends a message to the array filler with the value 1 - this
means he is ready to start and after he gets a message that it is possible to
start, he starts.

The manufacturer of the numbers in an endless loop of get random numbers in the
range 2..1000, and if the number is prime it sent to the array filler along 
with his ID (via message queue).

When the manufacturer receives a message from the array filler to end the 
program, it shows how many entries he grilled were new in the array and also
how many times it appeared in the array the entry he sent him and appeared 
most often in the array

Input: no input from user only from queue (as writen above)
Output: 
		Num of new primes NUM_OF_NEW_PRIME
		The prime with max appearance in arr is PRIME_APPEAR MOST it appear
		APPEARENCE_IN_ARRAY times (in one line)

compile: gcc -Wall ex4b2.c -o ex4b2
run: ./ex3b2

importent: must to run ex4b2.c befor this program!


Program file: ex4c1.c
===============================================================================
This program is a server that keeps process in system. 
The server generates a queue of messages.

The server allows three operations:
1. Adding a new process.
2. Checking whether a process is registered in the system.
3. Removal of an existing process.

When he receives a message requesting to add a process, he checks if process 
us exists in data base. If the process does not exist, and there is a place in
the data base its adds the process.
It responds the message: 0 if the process was successfully added, 1 if the 
process did not add since it already exists and 2 if the process did not add
since the data base is full.

When he receives a message asking to check if he has a specific process, he 
checks if the process number sent to it is stored in the data base, and 
returns: 1 if it exists, 0 if it does not exist.

When he receives a message requesting to remove a process, he removes it from
his data base.

When it receives the SIGINT signal, it turns to the handler signal,
releases the queue, and finishes.

Input:	 
Output:	 

compile: gcc -Wall ex4c1.c -o ex4c1
run: ./ex3c1

importent: must to run it befor program in ex4c1.c!


Program file: ex4c2.c
===============================================================================
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

Input:	 
Output:	 

compile: gcc -Wall ex4c2.c -o ex4c2
run: ./ex3c2

importent: must to run it befor program in ex4c2.c!


Program file: ex4c3.c
===============================================================================
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

Input: 'n' and after \n or ' ' a number OR 's' and after \n or ' ' a string
Output: 

compile: gcc -Wall ex4c3.c -o ex4c3
run: ./ex4c3  

importent: to run this program we must compile and run the programs in files
ex4c3.c ex4c3.c befor!
