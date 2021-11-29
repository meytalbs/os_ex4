
// --------------------------------includes------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//--------------------------------global variables------------------------------
const int NUM_OF_ARG = 3;
#define m_queue 2
#define main_process_name 1
#define GO 1

struct data {
    int _max_prime_appearance;
    int _new_prime;
};

// -----------------------------------------------------------------------------
int get_prime();
int is_prime(int number);
void foo(char* argv[], FILE** to_send, FILE** to_read);
void do_process(int seed, FILE** to_send, FILE** to_read);

// -------------------------------main------------------------------------------
int main(int argc, char* argv[])
{
    if (NUM_OF_ARG != argc)
        exit(EXIT_FAILURE);

    int seed = atoi(argv[m_queue]);			//num to srand & pipe num
    FILE* to_send, * to_read;
    foo(argv, &to_send, &to_read);
    do_process(seed, &to_send, &to_read);

    return EXIT_SUCCESS;
}
//------------------------------------------------------------------------------
void foo(char* argv[], FILE** to_send, FILE** to_read)
{
    char my_pipe[6] = "fifo";
    strcat(my_pipe, argv[m_queue]);
    if ((mkfifo(my_pipe, S_IFIFO | 0644) == -1) && errno != EEXIST)
        exit(EXIT_FAILURE);

    *to_send = fopen(argv[main_process_name], "w");

    fprintf(*to_send, "%d ", GO);	//started running
    fflush(*to_send);

    *to_read = fopen(my_pipe, "r");
    if (!*to_send || !*to_read)
        exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------------
int get_prime()
{
    int num;
    while (!is_prime(num = rand() % 999 + 2)) {}

    return num;
}

//------------------------------------------------------------------------------
// this function gets number and return 1 if its a prime number and 0 if not
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

// -----------------------------------------------------------------------------
void do_process(int seed, FILE** to_send, FILE** to_read)
{
    int prime, num_of_appearance;
    srand(seed);
    struct data my_data = { 0 };		//{max appearance, new prime}
    while (1)
    {
        prime = get_prime();
        fprintf(*to_send, "%d %d\n", prime, seed);
        fflush(*to_send);

        fscanf(*to_read, "%d", &num_of_appearance);
        if (0 == num_of_appearance)
            ++my_data._new_prime;
        else if (-1 == num_of_appearance)		//main process asked to stop
            break;
        if (num_of_appearance > my_data._max_prime_appearance) //saves max
            my_data._max_prime_appearance = num_of_appearance;
    }

    printf("num of new primes = %d\nmax num of appearances of a prime = %d\n",
        my_data._new_prime, my_data._max_prime_appearance);
}
