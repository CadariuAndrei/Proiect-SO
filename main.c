#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "treasure_hunt.h"
int main(int argc,char ** argv)
{
    if(argc!=3)
    {
        perror("Error at command line argumets");
        exit(-1);
    }
    char *operation=argv[1];
    if(strcmp(operation,"-add")==0)
    {
       add_treasure(argv[2]);
    }
    return 0;
}