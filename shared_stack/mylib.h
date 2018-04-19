#include<stdio.h>
#include<stdlib.h>
#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <sys/wait.h> /* for wait() */
#include <sys/types.h> /* for wait() kill(2)*/
#include <sys/ipc.h> /* for shmget() shmat() */
#include <sys/shm.h> /* for shmget() shmat() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h> //exit
#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include <stdbool.h>//bool
#ifndef MY_LIB_H_
#define MY_LIB_H_
//#define MAX 10
//#define max_element 10
//***************************structure*******************************************//
struct stack{
key_t stack_key;
int data_size;
int stack_size;
void *stack_top;
int no_element;
int no_stack;
bool free;
};

int initialise();
int shstackget(key_t key,int data_size, int stack_size);
int shstackpush(int stack_no,int data);
void shstackrm(int pos);
int shstackpop(int stack_no);
void shclose();
#endif
