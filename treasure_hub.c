#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>

#define COMMAND_FILE "command.txt"
pid_t monitor_pid=-1;
int monitor_exit=0;

void start_monitor()
{
    if(monitor_pid!=-1)
    {
        printf("Monitor is already running\n");
        return;
    }
    pid_t pid=fork();
    if(pid==0)
    {
        execl("./monitor","./monitor",NULL);
        exit(1);
    }
    else
    {
        monitor_pid=pid;
        printf("Monitor started\n");
    }
}

void stop_monitor()
{
    if(monitor_pid==-1)
    {
        printf("No monitor is running\n");
        return;
    }
    monitor_exit=1;
    kill(monitor_pid,SIGTERM);
    int status;
    waitpid(monitor_pid,&status,0);
    printf("Monitor terminated with status %d\n",WEXITSTATUS(status));
    monitor_pid=-1;
    monitor_exit=0;
}

void exit_monitor()
{
    if(monitor_pid!=-1)
    {
        fprintf(stderr,"Monitor is still running.\n");
        return;
    }
    printf("Exit treasure hub\n");
    exit(0);
}

void list_hunts()
{
    kill(monitor_pid,SIGUSR1);
    sleep(1);
}

void list_treasures()
{
    char hunt_id[32];
    printf("Enter hunt id: ");
    fgets(hunt_id,sizeof(hunt_id),stdin);
    hunt_id[strcspn(hunt_id,"\n")]=0;
    
    FILE *fp=fopen(COMMAND_FILE,"w");
    if(fp)
    {
        fprintf(fp,"%s\n",hunt_id);
        fclose(fp);
        kill(monitor_pid,SIGUSR2);
        sleep(1);
    }
    else
    {
        perror("Failed to write cu commnad file");
    }
}

void view_treasure()
{
    char hunt_id[32];
    char treasure_id[64];
    printf("Enter hunt ID: ");
    fgets(hunt_id,sizeof(hunt_id),stdin);
    hunt_id[strcspn(hunt_id,"\n")]=0;
    printf("Enter treasure ID: ");
    fgets(treasure_id,sizeof(treasure_id),stdin);
    treasure_id[strcspn(treasure_id,"\n")]=0;
    FILE *fp=fopen(COMMAND_FILE,"w");
    if(fp)
    {
        fprintf(fp,"%s %s\n",hunt_id,treasure_id);
        fclose(fp);
        kill(monitor_pid,SIGINT);
        sleep(1);
    }
    else
    {
        perror("Failed to write cu commnad file");
    }
    
}
int main()
{
    char cmd[128];
    while(1)
    {
        printf("treasure_hub > ");
        fflush(stdout);
        if(fgets(cmd,sizeof(cmd),stdin)==NULL)
        {
            break;
        }
        cmd[strcspn(cmd,"\n")]=0;
        if(strcmp(cmd,"start_monitor")==0)
        {
            start_monitor();
            sleep(1);
        }
        else if(strcmp(cmd,"stop_monitor")==0)
        {
            stop_monitor();
        }
        else if(strcmp(cmd,"exit")==0)
        {
            exit_monitor();
        }
        else if(strcmp(cmd,"list_hunts")==0)
        {
           
            list_hunts();
        }
        else if(strcmp(cmd,"list_treasures")==0)
        {
            list_treasures();
        }
        else if(strcmp(cmd,"view_treasure")==0)
        {
            view_treasure();
        }
        else if(monitor_exit)
        {
            printf("[!] Cannot accept commnads until monitor has exited.\n");
        }
        else
        {
            printf("[!] Unknown command:%s\n",cmd);
        }
    }
    return 0;
}