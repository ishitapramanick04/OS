#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <dirent.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

#define LEN 20
#define STD_IN 0
#define STD_OUT 1
char subcommand[20][20];
void sub_commands(char command[])
{
  int i=0,j=0,k=0,x=0;

  bzero(subcommand,sizeof(subcommand));
  while(1)
  {

    if(command[i]=='\0')
    {
      break;
    }
    else if(command[i]==' ')
    {
      subcommand[j][k]='\0';
      j++;
      k=0;
    }
    else
    {
      subcommand[j][k]=command[i];
      k++;
    }
    i++;
  }
}
int cd(int offset)
{
  char *pwd = getenv("PWD");
  //printf("%s \n",pwd);
  if(strcmp(subcommand[offset+1],"..")==0)
  {
    int len=strlen(pwd);
    for(int i=len-1;i>=0;i--)
    {
      if(pwd[i]=='/')
      {
        pwd[i]='\0';
        break;
      }
      pwd[i]='\0';
    }
  }
  else
  {
   strcat(pwd,"/");
   strcat(pwd,subcommand[offset]);
  }
  int dir=chdir(pwd);
  if(dir<0)
  {
    int len=strlen(pwd);
    for(int i=len-1;i>=0;i--)
    {
      if(pwd[i]=='/')
      {
        pwd[i]='\0';
        break;
      }
      pwd[i]='\0';
    }
    //printf("%s \n",pwd);
    perror("error ");
    return -1;
  }
  //printf("%s \n",pwd);
  return 0;
}

int external_command(int i, int offset)
{
  i--;
  int status=0;
  pid_t x= fork();
  pid_t pp;
  char *args[10];
  //char add[20]="/bin/";
  //args[0]=add;
  //strcat(add,subcommand[offset]);
  //args[0]=add;
  //args[1]=NULL;
  int a=0;
  for(int j=offset;j<=i;j++)
  {
    args[a]=subcommand[j];
    a++;
  }
  args[a]=NULL;
   if (x==0)
      {
         status=execvp(args[0],args);
         perror("error:");

       }
          else if((pp =wait(&status))>0)
          {
            if (WIFEXITED(status)!=0)
            {
              //printf("Exit status: %d\n", WEXITSTATUS(status));
              return WEXITSTATUS(status);
            }
            return 0;
          }

     return 0;
}
int runpipe(int o, int s, int offset, int i)
{
  int pipefd[2];//store file descriptors
	pipe(pipefd);
  int stdin,stdout;
  int status=0;
  pid_t x= fork();
  pid_t x1,pp;
  char *args[10];
  //char add[20]="/bin/";
  //args[0]=add;
  //strcat(add,subcommand[o]);
  //args[0]=add;
  //args[1]=NULL;
  int a=0;
  for(int j=o;j<=s;j++)
  {
    args[a]=subcommand[j];
    a++;
  }
  args[a]=NULL;

  ///////////////////////second
  i--;
  char *args1[10];
  char add1[20]="/bin/";
  //args[0]=add;
//  strcat(add1,subcommand[offset]);
//  args1[0]=add1;
  //args[1]=NULL;
  int b=0;
  for(int j=offset;j<=i;j++)
  {
    args1[b]=subcommand[j];
    b++;
  }
  args1[b]=NULL;
   if (x==0)
      {
          //wait(NULL);
        //child 1 for 1st process
		     /// close(pipefd[0]); //read end close
          printf("\r");
          //stdout=dup(STDOUT_FILENO);
          printf("\r");
		      dup2(pipefd[1], STDOUT_FILENO);
          printf("\r");
          execvp(args[0],args);
          perror("error1");
          return -1;
       }
       else{
        // wait(NULL);
         //separate process from 1st process
		      x1 = fork();//LHS of grep
          if (x1 == 0)
            {
			          // close(pipefd[1]); //write end close
                 printf("\r");
                 //stdin=dup(STDIN_FILENO);
                 printf("\r");
			           dup2(pipefd[0], STDIN_FILENO);
                 printf("\r");
                 close(pipefd[1]);
			           status=execvp(args1[0], args1);
                 perror("error2");
                 return -1;
               }
               else
               {
                 wait(NULL);
                 close(pipefd[1]);
                 close(pipefd[0]);
                 printf("\r");
                 //dup2(stdout,STDOUT_FILENO);
                 printf("\r");
                 //dup2(stdin,STDIN_FILENO);
                 printf("\r");
               } /*else if((pp =wait(&status))>0)
                 {
                    if (WIFEXITED(status)!=0)
                    {
                    //printf("Exit status: %d\n", WEXITSTATUS(status));
                    return WEXITSTATUS(status);
                    }
          } */

      }

  return 0;
}
int selecttype()
{
 int o=0,s=0; //for pipe
 int flag_in=0,flag_out=0; // for IO redirection
 int stdin,stdout;
 int fd_in,fd_out;
 int i=0;
 int ret;
 //int start=0,stop=0;
 while(strlen(subcommand[i])>0)
 {
   if(strcmp(subcommand[i],"cd")==0)
   {
     ret=cd(i);
     i+=2;
   }
   else if(strcmp(subcommand[i],"pwd")==0)
   {
     char *pwd = getenv("PWD");
     printf("%s \n",pwd);
     ret=0;
     i++;
   }
   else if(strcmp(subcommand[i],"clear")==0)
   {
     printf("\033[H\033[J");
     ret=0;
     i++;
   }
   else if(strcmp(subcommand[i],"exit")==0)
   {
     printf(BOLDBLUE"(END)"RESET);
     ret=0;
     exit(0);
   }
   else if(strcmp(subcommand[i],";")==0)
   {
     i++;
     continue;
   }
   else if(strcmp(subcommand[i],">")==0)
   {

     fd_out= open(subcommand[i+1],O_CREAT |  O_RDWR |O_TRUNC ,00700);
     if(fd_out==-1)
     {
       printf("failed to open\n");
    //   ret=1;
       return 0;
     }
     stdout=dup(STDOUT_FILENO);
     printf("\r");
     dup2(fd_out,STDOUT_FILENO);
     printf("\r");
     flag_out=1;
     i+=2;
   }
   else if(strcmp(subcommand[i],">>")==0)
   {

     fd_out= open(subcommand[i+1],O_CREAT | O_APPEND | O_RDWR ,00700);
     if(fd_out==-1)
     {
       printf("failed to open\n");
      // ret=1;
       //return 0;
     }
     stdout=dup(STDOUT_FILENO);
     printf("\r");

     dup2(fd_out,STDOUT_FILENO);
     printf("\r");

     flag_out=1;
     i+=2;
   }
    else if(strcmp(subcommand[i],"<")==0)
    {

      fd_in= open(subcommand[i+1],O_CREAT | O_RDWR | O_TRUNC ,00700);
      if(fd_in==-1)
      {
        printf("failed to open\n");
      //  ret=1;
        //return 0;
      }
      stdin=dup(STDIN_FILENO);
      printf("\r");

      dup2(fd_in,STDIN_FILENO);
      printf("\r");

      flag_in=1;
      i+=2;
    }
     else if(strcmp(subcommand[i],"<<")==0)
     {

       fd_in= open(subcommand[i+1],O_CREAT | O_APPEND | O_RDWR ,00700);
       if(fd_in==-1)
       {
         printf("failed to open\n");
      //   ret=1;
         //return 0;
       }
       stdin=dup(STDIN_FILENO);
       printf("\r");

       dup2(fd_in,STDIN_FILENO);
       printf("\r");

       flag_in=1;
       i+=2;
     }
   else if(strcmp(subcommand[i],"||")==0)
   {
    if(ret==0)
    {
      printf("previous operation was successful: %s \n",subcommand[i]);
      break;
    }
    i++;
   }
   else if(strcmp(subcommand[i],"&&")==0)
   {
     if(ret!=0)
     {
       printf("previous operation was unsuccessful: %s \n",subcommand[i]);
       break;
     }
     i++;

   }
   else if(strcmp(subcommand[i],"|")==0)
   {
     i++;
     s--;
     int offset=i;
     //for(int z=o;z<=s;z++)
     //printf("%s\n",subcommand[z]);
     //printf("offset: %d",offset);
     while((strcmp(subcommand[i],";")!=0)
      && (strcmp(subcommand[i],"&&")!=0)
      && (strcmp(subcommand[i],"||")!=0)
      && strlen(subcommand[i])>0
      && (strcmp(subcommand[i],"|")!=0)
      && (strcmp(subcommand[i],"<")!=0)
      && (strcmp(subcommand[i],"<<")!=0)
      && (strcmp(subcommand[i],">>")!=0)
      && (strcmp(subcommand[i],">>")!=0))
     {
       //printf("subcommand:%s\n",subcommand[i]);
       i++;

     }
     //ret=external_command(i,offset);
     //printf("final: %d",i);
    // printf("ret: %d\n",ret);
     ret=runpipe(o,s,offset,i);
     //i++;
   }
   else if(strlen(subcommand[i])>0)
   {
     int offset=i;
     o=offset;
     //printf("offset: %d",offset);
     while((strcmp(subcommand[i],";")!=0)
      && (strcmp(subcommand[i],"&&")!=0)
      && (strcmp(subcommand[i],"||")!=0)
      && strlen(subcommand[i])>0
      && (strcmp(subcommand[i],"|")!=0)
      && (strcmp(subcommand[i],"<")!=0)
      && (strcmp(subcommand[i],"<<")!=0)
      && (strcmp(subcommand[i],">>")!=0)
      && (strcmp(subcommand[i],">>")!=0))
     {
       //printf("subcommand:%s\n",subcommand[i]);
       i++;

     }
     s=i;
     ret=external_command(i,offset);
     //printf("final: %d",i);
    // printf("ret: %d\n",ret);
   }

 }
 if(flag_in)
 {
  dup2(stdin,STDIN_FILENO);
  printf("\r");

 }
 if(flag_out)
 {
  dup2(stdout,STDOUT_FILENO);
  //printf("boo");
  printf("\r");

 }
 return 0;
}

int main()
{
  system("clear"); //clears the screen
  printf(CYAN"--------------------------------------------------------------------------------"RESET);
  printf("\n\n\t\t\t"BOLDRED"I S H I T A ' S     S H E L L"RESET"\n\n");
  printf(CYAN"--------------------------------------------------------------------------------\n"RESET);
  while(1)
  {
   char command[200];
   int i=0;
   bzero(command,sizeof(command));
   struct utsname sysinfo;
   uname(&sysinfo);
   char *pwd = getenv("PWD");
   printf("\n");
   printf(BOLDGREEN"%s"RESET":"BOLDBLUE"~%s"RESET"$ ",sysinfo.nodename,pwd);
   while(1)
   {
     char c;
     scanf("%c",&c);
     if(c=='\n')
     {
       command[i]='\0';
       break;
     }
     else
       command[i]=c;
       i++;
   }
   sub_commands(command);
   selecttype();
  }

  return 0;
}
