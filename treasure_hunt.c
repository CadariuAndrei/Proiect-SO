#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include "treasure_hunt.h"
#include<unistd.h>
Treasure read_treasure()
{
    Treasure t;

    printf("Enter treasure id:");
    scanf("%d",&t.treasure_id);
    getchar();

    printf("Enter treasure user name:");
    fgets(t.user_name,MAX_USERNAME_SIZE,stdin);
    t.user_name[strcspn(t.user_name,"\n")]='\0';

    printf("Enter treasure latitude:");
    scanf("%lf",&t.latitude);
    getchar();

    printf("Enter treasure longitude:");
    scanf("%lf",&t.longitude);
    getchar();

    printf("Enter treasure clue text:");
    fgets(t.clue_text,MAX_CLUETEXT_SIZE,stdin);
    t.clue_text[strcspn(t.clue_text,"\n")]='\0';

    printf("Enter treasure value:");
    scanf("%d",&t.value);
    getchar();

    return t;
}

int existing_directory(char *hunt_id)
{
    struct stat st;
    if(stat(hunt_id,&st)==0 && S_ISDIR(st.st_mode))
    {
        return 1;
    }
    else
    {
    return 0;
    }
}
void add_treasure(char *hunt_id)
{
   Treasure t;
   t=read_treasure();
   if(existing_directory(hunt_id)==0)
   {
     if(mkdir(hunt_id,0777)==-1)
     {
        perror("Error creating directory");
        exit(-1);
     }
   }
   char treasure_hunt_path[256];
   sprintf(treasure_hunt_path,"%s/treasures.dat",hunt_id);
   int fd=open(treasure_hunt_path,O_WRONLY | O_CREAT | O_APPEND, 0777);
   if(fd==-1)
   {
    perror("Error creating/opening file");
    exit(-1);
   }
   if(write(fd,&t,sizeof(t))!=sizeof(t))
   {
    perror("Error writing to file");
    close(fd);
    exit(-1);
   }
   close(fd);
}
