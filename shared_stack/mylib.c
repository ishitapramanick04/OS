#include"mylib.h"
//#include<stdio.h>
//#include<stdlib.h>
//#include <stdio.h>
//#include <unistd.h> /* for fork() */
//#include <sys/wait.h> /* for wait() */
//#include <sys/types.h> /* for wait() kill(2)*/
//#include <sys/ipc.h> /* for shmget() shmat() */
//#include <sys/shm.h> /* for shmget() shmat() */
//#include <signal.h> /* for signal(2) kill(2) */
//#include <errno.h> /* for perror */
//#include <stdlib.h> //exit
//#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
//#include <stdbool.h>//bool
#define MAX 10
#define max_element 10
//***************************structure*******************************************//
/*struct stack{
key_t stack_key;
int data_size;
int stack_size;
void *stack_top;
int no_element;
int no_stack;
bool free;
}; */
struct stack *stackdesc;
int shmid; //stack descriptor
int shmid1;//each shared stack has a different shmid1
struct stack *p; //stack descriptors
int *pi; //stackpos
int initialise()
{
  size_t tots=sizeof(int)*MAX*sizeof(struct stack);
  key_t mykey=ftok("/home/ishita/Documents/os/a5/shm.txt",11);
  shmid = shmget(mykey,tots,IPC_CREAT|IPC_EXCL|0777);
  if(shmid<0)
  {
    //for the next subsequent times
    shmid = shmget(mykey,tots,IPC_CREAT | 0777);
    if (shmid == -1) { /* shmget() failed() */
                perror("shmget() failed: ");
                exit (1);
        }
    p = (struct stack *)shmat(shmid, NULL, 0);
    return 0;
  }
  p = (struct stack *)shmat(shmid, NULL, 0);

  // It will come here only once
//  printf("hello\n");
  for(int i=0;i<MAX;i++)
  {
      (p+i)->free=true;
  }
  return 0;
  }
int shstackget(key_t key,int data_size, int stack_size)
{
    //int shmid;
      initialise();
  //    printf(" 1 ");
      int pos=-1; //initialise pos with 1
      int i=(int)key;
      if((p+i)->free==true)
      pos=i;
  //    printf("pos %d i %d",pos,i);
      if(pos==-1)
      {
        printf("you have to share the stack\n");
        key_t mykey=ftok("/home/ishita/Documents/os/a5/text.txt",i);
        pos=i;
        size_t tots=sizeof(sizeof((p+pos)->stack_size*(p+pos)->data_size));
        shmid1 = shmget(mykey,tots, 0777);
        if (shmid1 == -1) { /* shmget() failed() */
                    perror("shmget() failed: ");
                    exit (1);
            }
        pi = (int *)shmat(shmid1, NULL, 0);
        (p+i)->no_stack++;
        return pos;
      }
      else
      {
        //size_t tots=sizeof(int)*sizeof(struct stack);;
        key_t mykey=ftok("/home/ishita/Documents/os/a5/text.txt",pos);
        shmid1 = shmget(mykey,sizeof(int)*data_size*stack_size,IPC_CREAT|IPC_EXCL|0777);
        if (shmid1 == -1) { /* shmget() failed() */
                    perror("shmget() failed: ");
                    exit (1);
            }
         pi = (int *)shmat(shmid1, NULL, 0);
        (p+i)->stack_key=key;
        // printf("%d ",(int)(p+i)->stack_key);
        (p+i)->data_size=data_size;
        (p+i)->stack_size=stack_size;
        (p+i)->stack_top=(void *)pi;
        // printf("%d ",(int)(p+i)->stack_top);
        (p+i)->no_element=0;
        (p+i)->free=false;
        (p+i)->no_stack=1;
    }
    return pos;
}
int shstackpush(int stack_no,int data)
{
      if(stack_no<0 && stack_no>9)
      {
        printf("invalid stack number\n");
        return -1;
      }
      //struct stack *stackid=(p+stack_no);
      //stackdesc=(struct stack*)stackid->stack_top;
      //struct stack *pi = (struct stack *)shmat(shmid1, NULL, 0);
      if((p+stack_no)->no_element==(p+stack_no)->stack_size)
      return -1;
      int *top=(void *)p->stack_top;
      top++;
      p->stack_top=top;
    //  *(int *)(p->stack_top)=data;
      pi++;
      *pi=data;
      p->stack_top=(void *)pi;
      p->no_element++;
      return 0;
}
 int shstackpop(int stack_no)
{
    if(stack_no<0 && stack_no>9)

     {
       printf("invalid stack number\n");
       return -1;
     }
     stackdesc=(p+stack_no);
     if(stackdesc->no_element==0)
     return -1;
     int data;
     int *top=stackdesc->stack_top;
     data=*top;
     top--;
     stackdesc->stack_top=top;
     stackdesc->no_element--;
     return data;
}
void shstackrm(int stack_no)
{
     if(stack_no<0 && stack_no>9)
     {
       printf("invalid stack number\n");
      // return -1;
     }
     stackdesc=(p+stack_no);
     if(stackdesc->no_stack==1)
     stackdesc->free=true;
     shmdt(pi);
}
void shclose()
{
    shmdt(pi);
    shmctl(shmid1, IPC_RMID, NULL);

    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    /*------------------------------------------------------------------------*/
}
/*
int main()
{
   int no,size,noelement;
   printf("enter the following details\n 1.number of stack ranges from 1-10 \n 2.data size(should be an integer)\n 3.number of elements(1-10)\n");
   printf("stack_no");
   scanf("%d",&no);
   printf("\ndata size");
   scanf("%d",&size);
   printf("\nnumber of elements");
   scanf("%d",&noelement);
   int y=shstackget(no,size,noelement);
   printf("on failure -1\n");
   printf("return value is : %d\n",y);
   if(y!=-1)
   {
     while(1)
     {

       int choice;
       printf(" 1.push\n 2.pop\n 3.exit\n");
       scanf("%d",&choice);
       if(choice==1)
       {
         int data;
         printf(" enter data");
         scanf("%d",&data );
         int x=shstackpush(y,data);

       }
       else if(choice==2)
       {
          int x=shstackpop(y);
          printf(" popped element %d\n",x);
       }
       else if(choice==3)
       {
          shstackrm(y);
          printf(" bye\n");
          break;
       }
     }
   }

   return 0;
}

*/
