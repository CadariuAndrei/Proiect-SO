#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "treasure_manager.h"
/*
    What you should introduce in command line for each function:
    
    | add_treasure          | argv[1]: --add                      | argv[2]: hunt_id     | argv[3]: NO ARGUMENT |
    | list                  |          --list                     |          hunt_id     |          NO ARGUMENT |
    | view                  |          --view                     |          hunt_id     |          treasure_id |
    | remove_treasure       |          --remove_treasure          |          hunt_id     |          treasure_id |
    | remove_treasure_hunt  |          --remove_treasure_hunt     |          hunt_id     |          NO ARGUMENT |
*/
int main(int argc,char ** argv)
{
    if(argc!=3 && argc!=4)
    {
        perror("Error at command line argumets!");
        exit(-1);
    }
    char *operation=argv[1];
    if(strcmp(operation,"--add")==0)
    {
       add_treasure(argv[2]);
    }
    else
    {
        if(strcmp(operation,"--list")==0)
        {
            list(argv[2]);
        } 
        else
        {
            if(strcmp(operation,"--view")==0)
            {
                 view(argv[2],argv[3]);
            }
            else
            {
                if(strcmp(operation,"--remove_treasure")==0)
                {
                    remove_treasure(argv[2],argv[3]);
                }
                else
                {
                    if(strcmp(operation,"--remove_treasure_hunt")==0)
                    {
                          remove_treasure_hunt(argv[2]);
                    }
                    else
                    {
                        printf("The user introduced an invalid operation!\n");
                    }
                }
            }
        }
    }
    return 0;
}