#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "treasure_manager.h"
#include <unistd.h>
#include <time.h>

#define MAX_PATH_SIZE 128
#define MAX_LOG_MESSAGE_SIZE 256
int exist_file(const char *filepath)
{
    struct stat st;
    return (stat(filepath,&st)==0 && S_ISREG(st.st_mode)!=0);

}

int exist_directory(const char *hunt_id)
{
    struct stat st;
    return (stat(hunt_id,&st)==0 && S_ISDIR(st.st_mode)!=0);
}

Treasure read_treasure()
{
    Treasure t;

    printf("Enter treasure id: ");
    fgets(t.treasure_id,MAX_ID_SIZE,stdin);
    t.treasure_id[strcspn(t.treasure_id,"\n")]='\0';

    printf("Enter treasure user name: ");
    fgets(t.user_name,MAX_USERNAME_SIZE,stdin);
    t.user_name[strcspn(t.user_name,"\n")]='\0';

    printf("Enter treasure latitude: ");
    scanf("%lf",&t.latitude);
    getchar();

    printf("Enter treasure longitude: ");
    scanf("%lf",&t.longitude);
    getchar();

    printf("Enter treasure clue text: ");
    fgets(t.clue_text,MAX_CLUETEXT_SIZE,stdin);
    t.clue_text[strcspn(t.clue_text,"\n")]='\0';

    printf("Enter treasure value: ");
    scanf("%d",&t.value);
    getchar();

    return t;
}

void print_treasure(Treasure t)
{
    printf("Treasure Id: %s\n",t.treasure_id);
    printf("Treasure User Name: %s\n",t.user_name);
    printf("Treasure latitude: %.2f\n",t.latitude);
    printf("Treasure longitude: %.2f\n",t.longitude);
    printf("Treasure clue text: %s\n",t.clue_text);
    printf("Treasure value: %d\n",t.value);
}

void log_operation(const char *hunt_id,const char *log_message)
{
    char log_filepath[MAX_PATH_SIZE];
    sprintf(log_filepath,"%s/logged_treasure_hunt.txt",hunt_id);
    int lfd=open(log_filepath,O_WRONLY | O_APPEND | O_CREAT,0644);
    if(lfd==-1)
    {
        perror("Error opening or creating log file!");
        exit(-1);
    }
    if(write(lfd,log_message,strlen(log_message))!=strlen(log_message))
    {
        perror("Error writing to log file!");
        close(lfd);
        exit(-1);
    }
    close(lfd);
    char log_symbolic_link[MAX_PATH_SIZE];
    sprintf(log_symbolic_link,"logged_hunt-%s",hunt_id);
    struct stat st;
    if(lstat(log_symbolic_link,&st)==-1)
    {
        if(symlink(log_filepath,log_symbolic_link)==-1)
        {
            perror("Error creating symbolic link!");
            exit(-1);
        }
    }
}

void add_treasure(const char *hunt_id)
{
   Treasure t;
   t=read_treasure();
   if(!exist_directory(hunt_id))
   {
     if(mkdir(hunt_id,0777)==-1)
     {
        perror("Error creating treasure_hunt_directory!");
        exit(-1);
     }
     char log_message[MAX_LOG_MESSAGE_SIZE];
     sprintf(log_message,"Treasure hunt %s was created\n",hunt_id);
     log_operation(hunt_id,log_message);
   }
   char treasure_filepath[MAX_PATH_SIZE];
   sprintf(treasure_filepath,"%s/treasures.dat",hunt_id);
   int fd=open(treasure_filepath,O_WRONLY | O_CREAT | O_APPEND, 0777);
   if(fd==-1)
   {
    perror("Error creating or opening treasure file!");
    exit(-1);
   }
   if(write(fd,&t,sizeof(t))!=sizeof(t))
   {
    perror("Error writing to file");
    close(fd);
    exit(-1);
   }
   char log_message2[MAX_LOG_MESSAGE_SIZE];
   sprintf(log_message2,"Treasure %s was added to treasure hunt %s\n",t.treasure_id,hunt_id);
   log_operation(hunt_id,log_message2);
   close(fd);
}

void list(const char *hunt_id)
{
    if(!exist_directory(hunt_id))
    {
        printf("The %s treasure directory does not exist!\n",hunt_id);
        exit(-1);
    }
    char treasure_filepath[MAX_PATH_SIZE];
    sprintf(treasure_filepath,"%s/treasures.dat",hunt_id);
    if(!exist_file(treasure_filepath))
    {
        printf("The %s treasure file does not exist!\n",treasure_filepath);
        exit(-1);
    }
    struct stat st;
    if(stat(treasure_filepath,&st)==-1)
    {
        perror("Error retriving treasure from file");
        exit(-1);
    }
    printf("Treasure hunt name: %s\n",hunt_id);
    printf("Total treasure file size: %ld bytes\n",st.st_size);
    printf("Last modification time of tresure file: %s\n",ctime(&st.st_mtime));
    int fd=open(treasure_filepath,O_RDONLY);
    if(fd==-1)
    {
        perror("Error opening treasure file!");
        exit(-1);
    }
    printf("The file contains the treasures:\n");
    Treasure t;
    while(read(fd,&t,sizeof(t))==sizeof(t))
    {
        printf("\n");
        print_treasure(t);
    }
    close(fd);
    char log_message[MAX_LOG_MESSAGE_SIZE];
    sprintf(log_message,"Listed all treasures from treasure hunt %s\n",hunt_id);
    log_operation(hunt_id,log_message);
}

void view(const char *hunt_id,const char *treasure_id)
{
    if(!exist_directory(hunt_id))
    {
        printf("The %s treasure directory does not exist!\n",hunt_id);
        exit(-1);
    }
    char treasure_filepath[MAX_PATH_SIZE];
    sprintf(treasure_filepath,"%s/treasures.dat",hunt_id);
    if(!exist_file(treasure_filepath))
    {
        printf("The %s treasure file does not exist!\n",treasure_filepath);
        exit(-1);
    }
    int fd=open(treasure_filepath,O_RDONLY);
    if(fd==-1)
    {
        perror("Error opening treasure file!");
        exit(-1);
    }
    char log_message[MAX_LOG_MESSAGE_SIZE];
    int ok=0;
    Treasure t;
    while(read(fd,&t,sizeof(t))==sizeof(t))
    {
        if(strcmp(t.treasure_id,treasure_id)==0)
        {
            
            print_treasure(t);
            sprintf(log_message,"Viewed treasure %s from treasure hunt %s\n",treasure_id,hunt_id);
            ok=1;
            break;
        }
    }
    close(fd);
    if(ok==0)
    {
        printf("Treasure %s was not found in the treasure hunt %s!\n",treasure_id,hunt_id);
        sprintf(log_message,"Failed to view treasure %s from treasure hunt %s\n",treasure_id,hunt_id);
    }
    log_operation(hunt_id,log_message);
}

void remove_treasure(const char *hunt_id,const char *treasure_id)
{
    if(!exist_directory(hunt_id))
    {
        printf("The %s treasure directory does not exist!\n",hunt_id);
        exit(-1);
    }
    char treasure_filepath[MAX_PATH_SIZE];
    sprintf(treasure_filepath,"%s/treasures.dat",hunt_id);
    if(!exist_file(treasure_filepath))
    {
        printf("The %s treasure file does not exist!\n",treasure_filepath);
        exit(-1);
    }
    int fd=open(treasure_filepath,O_RDWR);
    if(fd==-1)
    {
        perror("Error opening treasure file!");
        exit(-1);
    }
    int ok=0;
    off_t rd_offset=0;
    off_t wr_offset=0;
    Treasure t;
    while(read(fd,&t,sizeof(t))==sizeof(t))
    {
        if(strcmp(t.treasure_id,treasure_id)==0)
        {
            ok=1;
            wr_offset=rd_offset;

        }
        else
        {
            if(ok)
            {
                lseek(fd,wr_offset,SEEK_SET);
                if(write(fd,&t,sizeof(t))!=sizeof(t))
                {   perror("Error writing to file!");
                    close(fd);
                    exit(-1);
                }

            }
            lseek(fd,rd_offset+sizeof(t),SEEK_SET);
            wr_offset=wr_offset+sizeof(t);
        }
        rd_offset=rd_offset+sizeof(t);
    }
    char log_message[MAX_LOG_MESSAGE_SIZE];
    if(ok)
    {
        if(ftruncate(fd,wr_offset)==-1)
        {
            perror("Error truncating treasure_file!");
            exit(-1);
        }
        else
        {
            sprintf(log_message,"The treasure %s was removed from treasure hunt %s\n",treasure_id,hunt_id);
        }
    }
    else
    {
        sprintf(log_message,"Failed to remove treasure %s  from treasure hunt %s!\n",treasure_id,hunt_id);
    }
    log_operation(hunt_id,log_message);
    close(fd);
}

void remove_treasure_hunt(const char *hunt_id)
{
    if (!exist_directory(hunt_id))
    {
        printf("The %s treasure directory does not exist!\n", hunt_id);
        exit(-1);
    }
    char treasure_filepath[MAX_PATH_SIZE];
    sprintf(treasure_filepath, "%s/treasures.dat", hunt_id);
    char old_logpath[MAX_PATH_SIZE];
    sprintf(old_logpath, "%s/logged_treasure_hunt.txt", hunt_id);
    char new_logpath[MAX_PATH_SIZE];
    struct stat st;
    if (stat("logs", &st) == -1)
    {   
         if (mkdir("logs", 0777) == -1)
            {
                perror("Error creating logs directory!");
                exit(-1);
            }
    }
    sprintf(new_logpath, "logs/%s.log", strrchr(hunt_id, '/') ? strrchr(hunt_id, '/') + 1 : hunt_id);
    if (exist_file(treasure_filepath))
    {
        if (unlink(treasure_filepath) == -1)
        {
            perror("Error removing treasure file!");
            exit(-1);
        }
    }
    if (exist_file(old_logpath))
    {
        if (rename(old_logpath, new_logpath) == -1)
        {
            perror("Error moving log file!");
            exit(-1);
        }
    }
    if (rmdir(hunt_id) == -1)
    {
        perror("Error removing hunt directory!");
        exit(-1);
    }
    printf("Treasure hunt %s was removed successfully! Log saved to %s\n", hunt_id, new_logpath);
}


