==============================================================================
Written by : Meytal Abrahamian  login : meytalben  id : 211369939
			 Tomer Akrish               tomerak         315224295
==============================================================================


Program file: ex4a1.c
===============================================================================
This program gets prime numbers from 3 diffrent manufactures and fills an array
every time the program gets a prime it will count how many time it had appear 
in the array and will send it as an answer to the manufactures who send it.
in addition, the program will count number of new primes, saves the latgest
and smallest prime and finally will print it.
the program comunicates with the manufactures using named pipes

input:		no input
output:		diversity, smallest value, biggest value

compile: gcc -Wall ex4a1.c -o ex4a1
run: ./ex4a1 fifo0 fifo1 fifo2 fifo3


Program file: ex4a2.c
===============================================================================
This program send to the filler program a prime number and wait for response
how many times has the prime appeared in the filler array if its the first 
appearance the program will count it and once the array is full will print
how many new numbers has it sent and which number he sent appeared the most

input:		no input
output:		diversity, most appeared value

compile: gcc -Wall ex4a2.c -o ex4a2
run: ./ex4a2 fifo0 1/2/3

you will first need to run "ex4a1" and only then this program


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
run: ./ex4b1  

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
run: ./ex4b2 1/2/3

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

Input:		*no input*
Output:		*no output*	 

compile: gcc -Wall ex4c1.c -o ex4c1
run: ./ex4c1

importent: (in 3 diffrent terminals) run order: ./ex4c1 ./ex4c2 ./ex4c3


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

Input:		*no input*
Output:		*no output*

compile: gcc -Wall ex4c2.c -o ex4c2
run: ./ex4c2

importent: (in 3 diffrent terminals) run order: ./ex4c1 ./ex4c2 ./ex4c3


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

Input: 'n' + number to check if he's prime OR 's' to check a string is pali
		OR 'e' to exit
Output: n: is the number prime: 1/0		s: is the string palindrome: 1/0

compile: gcc -Wall ex4c3.c -o ex4c3
run: ./ex4c3

importent: (in 3 diffrent terminals) run order: ./ex4c1 ./ex4c2 ./ex4c3

