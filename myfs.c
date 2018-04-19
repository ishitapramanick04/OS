#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
# define lli long long int
#define max_sc 20 // no of sub command
#define max_scl 20 // sub command length
#define max_sp 1  //suppaablock
#define max_inode 32
#define max_db 1024//datablock per inode
#define max_db_inode 64
char subcommand[max_sc+1][max_scl+1];
int is_mounted=0;
struct superblock{
  lli file_size;
  lli free_superblock;
  lli block_size;
  lli inode_size;
  lli inode_count;
  lli free_inode;
  lli inode_start;
  lli datablock_count;
  lli free_datablock;
  lli datablock_start;
  lli inode_map[max_inode];
  lli db_map[max_db];
};
struct superblock super_block;
struct inode
{
  char type;
  lli size;
  unsigned char filename[20];
  lli datablock_count;
  lli no_db[max_db_inode];
};
struct inode in;
struct mount{
  unsigned char filename[20];
  unsigned char drivename[20];
  int fd;
  struct superblock sb;
  struct mount *next;
};
struct mount *moo_point;


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
void create_file()
{

  int  size,bs;
  int op= open(subcommand[1],O_RDWR | O_CREAT,00700);
  if(op==-1)
  {
    fprintf(stderr,"CANNOT OPEN FILE\n");
    return ;
  }
  size=atoi(subcommand[3]);
  //size=size*1024*1024;
  if(strcmp(subcommand[4],"mb")==0)
    size=size*1024*1024;
  if(strcmp(subcommand[4],"kb")==0)
    size=size*1024;
  int filesize=ftruncate(op,size);
//  printf("%lld %lld ",sizeof(struct superblock),sizeof(struct inode));
  //write in super_block
  super_block.file_size=filesize;
  super_block.free_superblock=max_sp;
  super_block.block_size=atoi(subcommand[2]);
  super_block.inode_size=sizeof(struct inode);
  super_block.inode_count=max_inode;
  super_block.free_inode=max_inode;
  super_block.inode_start=sizeof(struct superblock)*max_sp;
  super_block.datablock_count=(size-sizeof(struct superblock)+ sizeof(struct inode)*max_inode)/super_block.block_size;
  super_block.free_datablock=super_block.datablock_count;
  super_block.datablock_start=(sizeof(struct superblock)*max_sp + sizeof(struct inode)*max_inode);

//  printf("data blocks number:%lld ",super_block.datablock_count);
  for (int i=0; i<max_inode; i++)
  {
    super_block.inode_map[i] = 0;
  }
  for (int i = 0; i<max_db; i++)
  {
    super_block.db_map[i] = 0;
  }
  super_block.free_superblock--;
//  super_block.free_inode--;
  //super_block.inode_map[0]=1;
//  printf("blocksize:%lld freedb:%lld\n",super_block.block_size,super_block.datablock_count);
  lseek(op,0,SEEK_SET);
  write(op,&super_block,sizeof(struct superblock));
  lseek(op,0,SEEK_SET);
  read(op,&super_block,sizeof(struct superblock));
//  printf("blocksize:%lld freedb:%lld\n",super_block.block_size,super_block.free_datablock);
  close(op);
  ///inode

}
void file_write()
{
  //  update in superblock
  int fd=open(subcommand[1],O_RDWR , 00700);
  //printf("%d",fd);
  lseek(fd,0,SEEK_SET);
  struct superblock sp;
  int sp_read=read(fd,&sp,sizeof(struct superblock));
  sp.free_inode--;
  sp.free_datablock--;
  sp.inode_map[0]=1;
  sp.db_map[0]=1;

  lseek(fd,0,SEEK_SET);
  write(fd,&sp,sizeof(struct superblock));
  //   update inode
  lseek(fd,sp.inode_start,SEEK_SET);
  struct inode node;
  int in_read=read(fd,&node,sizeof(struct inode));
  node.type='t';
  node.size=3;
  strcpy(node.filename,"xyz");
  //printf("%s\n",node.filename);
  node.no_db[0]=1;
  lseek(fd,sp.inode_start,SEEK_SET);
  write(fd,&node,sizeof(struct inode));
  //checking
  lseek(fd,sp.inode_start,SEEK_SET);
  read(fd,&node,sizeof(struct inode));
  //printf("%c %s\n",node.type,node.filename);

  //write in datablock
  lseek(fd,sp.datablock_start,SEEK_SET);
  char buffer[20];
  for(int i=0;i<20;i++)
  buffer[i]='a';

  lseek(fd,sp.datablock_start,SEEK_SET);
  int db_write=write(fd,&buffer,sizeof(buffer));

}

void mount_file()
{
  //printf("hello\n");
  struct mount *fs;
  fs=(struct mount*)malloc(sizeof(struct mount));
  strcpy(fs->drivename,subcommand[3]);
  strcpy(fs->filename,subcommand[1]);
//  printf(" %s ",fs->filename);
  fs->next=NULL;
  fs->fd=open(fs->filename,O_RDWR , 00700);
  lseek(fs->fd,0,SEEK_SET);
  int sp_read=read(fs->fd,&fs->sb,sizeof(struct superblock));
  if(!is_mounted)
  {
    moo_point=fs;
    is_mounted=1;
  }
  else{
    struct mount *temp;
    temp=moo_point;
    while(temp->next!=NULL)
    temp=temp->next;
    temp->next=fs;
  }
    close(fs->fd);
/*  struct mount *temp;
  temp=moo_point;
  while(1)
  {
    printf(" %s %s \n",temp->filename,temp->drivename);
    if(temp->next==NULL)
    break;
    else
    temp=temp->next;
  } */
}
int unmount_file()
{
  if(!is_mounted)
  {
    printf("No mounted file\n");
    return 0;
  }
  struct mount *temp,*fs;
  temp=moo_point;
  while(temp!=NULL)
  {
    fs=temp;
    temp=temp->next;
    printf("drive %s is unmounted successfully\n",fs->drivename);
    free(fs);
  }
  return 0;
}
int filesystem()
{
  struct mount *temp;
  temp=moo_point;
  if(!is_mounted)
  {
    printf("No filesystems are available for use \n");
    return 0;
  }
  printf("available filesytems: \n");
  while(1)
  {
    printf(" %s %s \n",temp->filename,temp->drivename);
    if(temp->next==NULL)
    break;
    else
    temp=temp->next;
  }
}
int ls()
{
  struct mount *temp;
  temp=moo_point;
//  printf("%s %s \n",temp->drivename,temp->filename);
  while(strcmp(temp->drivename,subcommand[1])!=0)
  {
    if(temp->next==NULL)
     {
       printf("DRIVE NOT FOUND\n");
        return 0;
      }
    else
     temp=temp->next;
  }
  struct superblock sp;
 int fd=open(temp->filename,O_RDWR , 00700);
 fd=temp->fd;
  lseek(fd,0,SEEK_SET);
  int sp_read=read(fd,&sp,sizeof(struct superblock));

  for(int i=0;i<max_inode;i++)
  {
    if(sp.inode_map[i])
     {
      // printf("hello\n");
       struct inode node;
       lseek(fd,sp.inode_start+i*sizeof(struct inode),SEEK_SET);
       int sp_read1=read(fd,&node,sizeof(struct inode));
       printf("filename: %d %s %lld \n",i,node.filename,sp.inode_start+i*sizeof(struct inode));

     }
  }
  close(fd);
  return 0;
}
int copy()
{
   int no=0; //to check which of the three cases
   if(subcommand[1][1]!=':' && subcommand[2][1]==':') //os to text
     no=1;
   else if(subcommand[1][1]==':' && subcommand[2][1]==':') //text to text
     no=2;
   else if(subcommand[1][1]==':' && subcommand[2][1]!=':') //text to os
     no=3;


  // 3 different cases
  switch(no)
  {
    case 1:  {
               //checking for free inode
                struct mount *temp;
                temp=moo_point;
                char drive[20];
                bzero(drive,sizeof(drive));
                drive[1]=':';
                drive[0]=subcommand[2][0];
                char file[20];
                bzero(file,sizeof(file));

                for(int i=2;i<strlen(subcommand[2]);i++)
                file[i-2]=subcommand[2][i];
              //  printf("drivename %s ",drive);
                while(strcmp(temp->drivename,drive)!=0)
                {
                  if(temp->next==NULL)
                  {
                    printf("DRIVE NOT FOUND\n");
                    return 0;
                  }
                  else
                  temp=temp->next;
                }
              //  printf("%s\n",temp->drivename);

                int fd=open(temp->filename,O_RDWR , 00700); //opening file on which to copy
                struct superblock sp;
                lseek(fd,0,SEEK_SET);
                int sp_read=read(fd,&sp,sizeof(struct superblock));
                int flag=0;
                for(int i=0;i<max_inode;i++)
                {
                  if(sp.inode_map[i]==0)
                   {
                     flag=1;
                     break;
                   }
                }
                if(flag==0)
                {
                  printf("Not enough space to copy \n");
                  return 0;;
                }
//........................from copy...........................
              int fd1=open(subcommand[1],O_RDWR , 00700); //opening os to copy
              lseek(fd1,0,SEEK_SET);
              struct superblock sp1;
              int sp_read1=read(fd1,&sp1,sizeof(struct superblock));

              int filesize=sp1.file_size;
              int block_size=sp1.block_size;

              for(int i=0;i<max_inode;i++)
              {
                if(sp1.inode_map[i])
                {
                  // copying data blocks
                  struct inode node;
                  lseek(fd1,sp1.inode_start+(i*sizeof(struct inode)),SEEK_SET);
                  int sp_read1=read(fd1,&node,sizeof(struct inode));
                  int count=node.datablock_count;
                  char buffer[block_size*count];
                  bzero(buffer,sizeof(buffer));
                  char store[block_size];
                //  printf("filename copy: %s ",node.filename);
                  for(int j=0;j<count;j++)
                  {
                    if(node.no_db[j])
                    {
                      lseek(fd1,j*block_size+sp1.datablock_start,SEEK_SET);
                      bzero(store,sizeof(store));
                      int db_read=read(fd1,&store,block_size);
                      strcat(buffer,store);
                    }

                  }

                     // copying into textfile
                    // lseek(fd,0,SEEK_SET);
                     //int sp_read=read(fd,&sp,sizeof(struct superblock));

                     int flag=0;
                     for(int j=0;j<max_inode;j++)
                     {
                       if(sp.inode_map[j]==0)
                        {
                          struct inode node1;
                          lseek(fd,sp.datablock_start+j*sp.block_size,SEEK_SET);
                          int db_write=write(fd,&buffer,strlen(buffer));
                          //printf("copy at %s ",buffer);
                          bzero(buffer,sizeof(buffer));
                          lseek(fd,sp.datablock_start+j*sp.block_size,SEEK_SET);
                          db_write=read(fd,&buffer,sizeof(buffer));
                        //  printf("buffer %d %s \n",db_write,buffer);
                          // writing into superblock
                          int no_block=ceil((float)strlen(buffer)/sp.block_size);
                          sp.free_datablock-=no_block;
                          sp.free_inode--;
                          sp.inode_map[j]=1;
                          for(int k=0;k<no_block;k++)
                          {
                            sp.db_map[j*max_db_inode+k]=1;
                          }
                          lseek(fd,0,SEEK_SET);
                          int sp_write=write(fd,&sp,sizeof(struct superblock));
                          //writing into inode
                          node1.type='t';
                          node1.size=strlen(buffer);
                          strcpy(node1.filename,file);
                          node1.datablock_count=no_block;
                          for(int k=0;k<no_block;k++)
                          node1.no_db[k]=1;
                          lseek(fd,sp.inode_start+j*(sizeof(struct inode)),SEEK_SET);
                          int in_write=write(fd,&node1,sizeof(struct inode));
                          //checkkk
                        //  printf("to be copied: %s ",node1.filename);
                          lseek(fd,sp.inode_start+j*(sizeof(struct inode)),SEEK_SET);
                          int in_read=read(fd,&node1,sizeof(struct inode));
                        //  printf("copy %s \n",node1.filename);
                          break;
                        }
                     }
                 }
              }
              close(fd);
              close(fd1);
    }
             break;
    case 2:
             {
// drive on which it is being copied
               struct mount *temp;
               temp=moo_point;
               char drive[20];
               bzero(drive,sizeof(drive));
               drive[1]=':';
               drive[0]=subcommand[2][0];
               char file[20];
               bzero(file,sizeof(file));
               for(int i=2;i<strlen(subcommand[2]);i++)
               file[i-2]=subcommand[2][i];
               //printf("drivename: %s file: %s \n",drive,file);
               while(strcmp(temp->drivename,drive)!=0)
               {
                 if(temp->next==NULL)
                 {
                   printf("DRIVE NOT FOUND2\n");
                   return 0;;
                 }
                 else
                 temp=temp->next;
               }
               int fd=open(temp->filename,O_RDWR , 00700); //opening file on which to copy
               fd=temp->fd;
               struct superblock sp;
               sp=temp->sb;
               int flag=0;
               for(int i=0;i<max_inode;i++)
               {
                 if(sp.inode_map[i]==0)
                  {
                    flag=1;
                    break;
                  }
               }
               if(flag==0)
               {
                 printf("Not enough space to copy \n");
                 return 0;;
               }

               //copy from drive

               char drive1[20];
               char file1[20];
               bzero(drive1,sizeof(drive1));
               drive1[0]=subcommand[1][0];
               drive1[1]=':';
               bzero(file1,sizeof(file1));
               for(int i=2;i<strlen(subcommand[1]);i++)
               file1[i-2]=subcommand[1][i];
               struct mount *temp1;
               temp1=moo_point;
               while(strcmp(temp1->drivename,drive1)!=0)
               {
                 if(temp1->next==NULL)
                  {
                    printf("DRIVE NOT FOUND\n");
                     return 0;;
                   }
                 else
                  temp1=temp1->next;
               }

                //printf("drive %s file %s temp_drive %s \n",drive1,file1,temp1->drivename);
                int fd1=open(temp1->filename,O_RDWR , 00700); //opening file on which to copy
                struct superblock sp1;
                lseek(fd1,0,SEEK_SET);
                int sp_read=read(fd1,&sp1,sizeof(struct superblock));
                int flag1=0;

                for(int i=0;i<max_inode;i++)
                {
                  if(sp1.inode_map[i])
                   {
                     struct inode node1;
                     lseek(fd1,sp1.inode_start+i*sizeof(struct inode),SEEK_SET);
                     int in_read1=read(fd1,&node1,sizeof(struct inode));
                    // printf("filename: %s \n",node1.filename);
                     if(strcmp(file1,node1.filename)==0)
                     {
                      // printf("hello\n");
                       flag1=1;
                       char buffer[node1.datablock_count*sp1.block_size];
                       bzero(buffer,sizeof(buffer));
                       for(int j=0;j<max_db_inode;j++)
                       {
                         char store[sp1.block_size];
                         bzero(store,sizeof(store));
                         if(node1.no_db[j])
                         {
                           lseek(fd1,sp1.datablock_start+j*sizeof(sp1.block_size),SEEK_SET);
                           int db_read=read(fd1,&store,sizeof(store));
                         }
                         strcat(buffer,store);
                       }
                      // printf("copy at :%s ",buffer);
                       for(int j=0;j<max_inode;j++)
                       {
                         if(sp.inode_map[j]==0)
                          {
                            struct inode node;
                            lseek(fd,sp.datablock_start+j*sp.block_size,SEEK_SET);
                            int db_write=write(fd,&buffer,strlen(buffer));
                            bzero(buffer,sizeof(buffer));
                            lseek(fd,sp.datablock_start+j*sp.block_size,SEEK_SET);
                            db_write=read(fd,&buffer,sizeof(buffer));
                          //  printf("buffer %s \n",buffer);
                            // writing into superblock
                            int no_block=ceil((float)strlen(buffer)/sp.block_size);
                            sp.free_datablock-=no_block;
                            sp.free_inode--;
                            sp.inode_map[j]=1;
                            for(int k=0;k<no_block;k++)
                            {
                              sp.db_map[j*max_db_inode+k]=1;
                            }
                            lseek(fd,0,SEEK_SET);
                            int sp_write=write(fd,&sp,sizeof(struct superblock));

                            //writing into inode

                            node.type='t';
                            node.size=strlen(buffer);
                            strcpy(node.filename,file);
                            //printf("filename: %s node: %s ",file,node.filename);                         node.datablock_count=no_block;
                            for(int k=0;k<no_block;k++)
                            node.no_db[k]=1;
                            lseek(fd,sp.inode_start+j*(sizeof(struct inode)),SEEK_SET);
                            int in_write=write(fd,&node,sizeof(struct inode));
                           // printf(" %d ",in_write);
                            //checkkk
                           // printf("to be copied: %s ",node.filename);
                            lseek(fd,sp.inode_start+j*(sizeof(struct inode)),SEEK_SET);
                            //printf(" %lld ",sp.inode_start+j*(sizeof(struct inode)));
                            int in_read=read(fd,&node1,sizeof(struct inode));
                            //printf("copy %d \n",in_read);
                            break;
                         }
                      }
                     }
                   }
                }
                if(!flag1)
                {
                  printf("FILE NOT FOUND\n");
                }
                close(fd);
                close(fd1);
             }
             break;
    case 3: {
      char drive1[20];
      char file1[20];
      bzero(drive1,sizeof(drive1));
      drive1[0]=subcommand[1][0];
      drive1[1]=':';
      bzero(file1,sizeof(file1));
      for(int i=2;i<strlen(subcommand[1]);i++)
      file1[i-2]=subcommand[1][i];
      struct mount *temp1;
      temp1=moo_point;
      while(strcmp(temp1->drivename,drive1)!=0)
      {
        if(temp1->next==NULL)
         {
           printf("DRIVE NOT FOUND\n");
            return 0;;
          }
        else
         temp1=temp1->next;
      }

       //printf("drive %s file %s temp_drive %s \n",drive1,file1,temp1->drivename);
       int fd1=open(temp1->filename,O_RDWR , 00700); //opening file on which to copy
       struct superblock sp1;
       lseek(fd1,0,SEEK_SET);
       int sp_read=read(fd1,&sp1,sizeof(struct superblock));
       int flag1=0;

       for(int i=0;i<max_inode;i++)
       {
         if(sp1.inode_map[i])
          {
            struct inode node1;
            lseek(fd1,sp1.inode_start+i*sizeof(struct inode),SEEK_SET);
            int in_read1=read(fd1,&node1,sizeof(struct inode));
           // printf("filename: %s \n",node1.filename);
            if(strcmp(file1,node1.filename)==0)
            {
              flag1=1;
              char buffer[node1.datablock_count*sp1.block_size];
              bzero(buffer,sizeof(buffer));
              for(int j=0;j<max_db_inode;j++)
              {
                char store[sp1.block_size];
                bzero(store,sizeof(store));
                if(node1.no_db[j])
                {
                  lseek(fd1,sp1.datablock_start+j*sizeof(sp1.block_size),SEEK_SET);
                  int db_read=read(fd1,&store,sizeof(store));
                }
                strcat(buffer,store);
              }
            //  printf(" copy %s ",buffer);
              int op= open(subcommand[2],O_RDWR | O_CREAT,00700);
              if(op==-1)
              {
                fprintf(stderr,"CANNOT OPEN FILE\n");
                return 0;
              }
              int size=10*1024*1024;
              int filesize=ftruncate(op,size);
              lseek(op,0,SEEK_SET);
              write(op,&buffer,sizeof(buffer));
              lseek(op,0,SEEK_SET);
              bzero(buffer,sizeof(buffer));
              read(op,&buffer,sizeof(buffer));
            //  printf("buffer: %s",buffer);
              close(op);
          }
        }
      }

      close(fd1);
            }
             break;
    default: printf("INVALID \n");
             break;
  }

}
int remove_file()
{
 char drive[20];
 char filename[20];
 bzero(drive,sizeof(drive));
 drive[0]=subcommand[1][0];
 drive[1]=':';
 bzero(filename,sizeof(filename));
 for(int i=2;i<strlen(subcommand[1]);i++)
 filename[i-2]=subcommand[1][i];
 struct mount *temp;
 temp=moo_point;
 while(strcmp(temp->drivename,drive)!=0)
 {
   if(temp->next==NULL)
    {
      printf("DRIVE NOT FOUND\n");
       return 0;;
     }
   else
    temp=temp->next;
 }
//printf("drive: %s file: %s ",temp->drivename,temp->filename);

// checking for the file
int fd=open(temp->filename,O_RDWR , 00700); //opening file on which to copy
fd=temp->fd;
struct superblock sp;
lseek(fd,0,SEEK_SET);
int sp_read=read(fd,&sp,sizeof(struct superblock));
//printf("block read: %d",sp_read);
for(int i=0;i<max_inode;i++)
{
  if(sp.inode_map[i])
   {
    // printf("hello\n");
     struct inode node;
     lseek(fd,sp.inode_start+i*sizeof(struct inode),SEEK_SET);
     int sp_read1=read(fd,&node,sizeof(struct inode));
     printf("file :%d\n",sp_read1);
    // printf("filename:%d %s %lld \n",i,node.filename,sp.inode_start+i*sizeof(struct inode));
    //printf("block read :%d ",in_read);
    printf("filename: %s ",node.filename);
    if(strcmp(filename,node.filename)==0)
    {
      int count=0;
      for(int j=0;j<max_db_inode;j++)
        {
          if(node.no_db[j])
           {
             node.no_db[j]=0;
             count++;
           }
        }
      //  write in inode
      lseek(fd,sp.inode_start+i*sizeof(struct inode),SEEK_SET);
      int ww=write(fd,&node,sizeof(struct inode));
      //printf(" %d ",ww);

      lseek(fd,0,SEEK_SET);
      sp.inode_map[i]=0;
      sp.free_datablock+=count;
      sp.free_inode++;
      ww=write(fd,&sp,sizeof(struct superblock));
    //  printf(" %d ",ww);
     break;
    }
  }
}
close(fd);
}
void move_file()
{
  copy(); //copy condition 2
  remove_file(); // remove the file in subcommand[1]
}
int main()
{
  while(1)
  {
    char command[200],c;
    printf("myfs>");
    bzero(command, sizeof(command));
    int i=0;
    while(1)
    {
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
  /*  for(int i=0;i<max_sc;i++)
    {
      for(int j=0;j<max_scl;j++)
      printf("%c",subcommand[i][j]);
      printf("\n");
    }*/
    if(strcmp(subcommand[0],"mkfs")==0)
     create_file();
    if(strcmp(subcommand[0],"use")==0)
     mount_file();
     if(strcmp(subcommand[0],"ls")==0)
     ls();
     if(strcmp(subcommand[0],"filesystem")==0)
     filesystem();
     if(strcmp(subcommand[0],"cp")==0)
     {
       //filesystem();
       copy();
     }
     if(strcmp(subcommand[0],"write")==0)
     {
       //filesystem();
       file_write();
     }
     if(strcmp(subcommand[0],"rm")==0)
     {
       //filesystem();
       remove_file();
     }
     if(strcmp(subcommand[0],"mv")==0)
     {
       //filesystem();
       move_file();
     }
    if(strcmp(subcommand[0],"exit")==0)
     {
       unmount_file();
       printf("exiting from file system..\n");
       return 0;
     }
  }
  return 0;
}
