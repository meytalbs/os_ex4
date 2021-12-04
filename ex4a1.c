/* ex4b.c
===============================================================================
Written by: Meytal Abrahamian  login: meytalben  id: 211369939
            Tomer Akrish              tomerak        315224295
===============================================================================
* This program gets prime numbers from 3 diffrent manufactures and fills an array
every time the program gets a prime it will count how many time it had appear
in the array and will send it as an answer to the manufactures who send it.
in addition, the program will count number of new primes, saves the latgest
and smallest prime and finally will print it.
the program comunicates with the manufactures using named pipes
*/

// --------------------------------includes------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//--------------------------------global variables------------------------------
#define ARR_SIZE 10000
#define NUM_OF_GEN 3
#define NUM_OF_PIPES 5

struct data {
    int _min_prime;
    int _max_prime;
    int _prime_diversity;
};

// -----------------------------------------------------------------------------
void create_pipes(char* argv[]);
void open_pipe_n_run_manu(FILE** from_filler, FILE* to_manufacture[],
    char* argv[]);
void fill_array(FILE* from_filler, FILE* to_manufacture[], struct data*);
int count_appear(int arr[], int num, int index);
void end_manufactures(FILE* to_manufacture[]);
void close_named_pipe(FILE* from_filler, FILE* to_manufacture[]);

// -------------------------------main------------------------------------------
int main(int argc, char* argv[])
{
    if (NUM_OF_PIPES != argc)
        exit(EXIT_FAILURE);

    struct data my_data = { 1000, 0, 0 }; //{min, max, appearance}
    FILE* fifo0, * m_fifo[NUM_OF_GEN];

    create_pipes(argv);
    open_pipe_n_run_manu(&fifo0, m_fifo, argv);
    fill_array(fifo0, m_fifo, &my_data);
    printf("Diversity: %d\nSmallest prime: %d\nBiggest prime: %d\n",
        my_data._prime_diversity, my_data._min_prime, my_data._max_prime);
    close_named_pipe(fifo0, m_fifo);

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// this function open pipe using pipe id
void create_pipes(char* argv[])
{
    if ((mkfifo(argv[1], S_IFIFO | 0644) == -1) && errno != EEXIST)
    {
        perror("cannot create fifo file");
        exit(EXIT_FAILURE);
    }
}

//------------------------------------------------------------------------------
// this function open pipe using pipe id
void open_pipe_n_run_manu(FILE** from_filler, FILE* to_manufacture[],
    char* argv[])
{
    int i, process_start;
    *from_filler = fopen(argv[1], "r");
    if (!*from_filler)
        exit(EXIT_FAILURE);

    for (i = 0; i < NUM_OF_GEN; ++i)
        fscanf(*from_filler, "%d", &process_start);

    for (i = 0; i < NUM_OF_GEN; ++i)
    {									// +2 for manu names (2, 3, 4)
        to_manufacture[i] = fopen(argv[i + 2], "w");
        if (!to_manufacture[i])
            exit(EXIT_FAILURE);
    }
}

//------------------------------------------------------------------------------
void fill_array(FILE* from_filler, FILE* to_manufacture[], struct data* my_data)
{
    int count_appearance = 0,		// count appearance of num in array
        i,							// for loop
        prime, id,
        arr[ARR_SIZE];				// array to fill

    for (i = 0; i < ARR_SIZE; ++i)
    {
        fscanf(from_filler, "%d%d", &prime, &id);
        arr[i] = prime;
        count_appearance = count_appear(arr, prime, i);

        if (count_appearance == 0)
            ++my_data->_prime_diversity;
        if (prime > my_data->_max_prime)
            my_data->_max_prime = prime;
        else if (prime < my_data->_min_prime)
            my_data->_min_prime = prime;

        fprintf(to_manufacture[id - 1], "%d ", count_appearance);
        fflush(to_manufacture[id - 1]);
    }

    end_manufactures(to_manufacture);
}

//------------------------------------------------------------------------------
// this function count appearance of num in array and returns the result
int count_appear(int arr[], int num, int index)
{
    int i, counter = 0;

    for (i = 0; i < index; ++i)
    {
        if (arr[i] == num)
            ++counter;
    }

    return counter;
}

//------------------------------------------------------------------------------
// this function kill the children
void end_manufactures(FILE* to_manufacture[])
{
    int i;

    for (i = 0; i < NUM_OF_GEN; ++i)
    {
        fprintf(to_manufacture[i], "%d ", -1);
        fflush(to_manufacture[i]);
    }
}

//------------------------------------------------------------------------------
// this function close pipe in parent
void close_named_pipe(FILE* from_filler, FILE* to_manufacture[])
{
    int i;

    fclose(from_filler);

    for (i = 0; i < NUM_OF_GEN; ++i)
        fclose(to_manufacture[i]);
}
