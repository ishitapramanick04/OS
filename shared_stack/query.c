#include"mylib.h"
int main()
{
  /* int y=shstackget(1,4,10);
   printf(" %d ",y);
   int x=shstackpush(0,1);
   printf("successful \n");
   y=shstackpop(0);
   printf("%d  boo ",y); */
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
       printf(" 1.push\n 2.pop\n 3.exit\n 4.remove\n");
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
       else if(choice==4)
       {
         shclose();
         printf("everything is removed\n");
         break;
       }
     }
   }

   return 0;
}
