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

Program file: ex4b2.c
===============================================================================

Program file: ex4c1.c
===============================================================================

Program file: ex4c2.c
===============================================================================

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