#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<dirent.h>

pid_t monitor_pid=-1;
int monitor_exiting=0;

int to_monitor[2];
int from_monitor[2];

void read_monitor_output()
{
    char buffer[1024];
    ssize_t bytes_read;
    usleep(200000);

    while((bytes_read=read(from_monitor[0],buffer,sizeof(buffer)-1))>0)
    {
        buffer[bytes_read]='\0';
        printf("%s",buffer);
        if(bytes_read<sizeof(buffer)-1)
        {
            break;
        }
    }
}

void send_data_to_monitor(const char *data)
{
    write(to_monitor[1],data,strlen(data));
}

void start_monitor()
{
    if(monitor_pid!=-1)
    {
        printf("Monitor is already running\n");
        return;
    }
    
    if(pipe(to_monitor)==-1 || pipe(from_monitor)==-1)
    {
        printf("Pipe error\n");
        exit(1);
    }

    pid_t pid=fork();

    if(pid==0)
    {
        dup2(to_monitor[0],STDIN_FILENO);
        dup2(from_monitor[1],STDOUT_FILENO);

        close(to_monitor[1]);
        close(to_monitor[0]);
        close(from_monitor[0]);
        close(from_monitor[1]);

        execl("./monitor","./monitor",NULL);
        printf("execl failed\n");
        exit(1);

    }
    else
    {
        if(pid>0)
        {
            monitor_pid=pid;
            close(to_monitor[0]);
            close(from_monitor[1]);

            printf("Monitor started (PID = %d)\n",monitor_pid);
        }
        else
        {
            printf("fork failed\n");
            exit(1);
        }
    }
}

void stop_monitor()
{
    if(monitor_pid==-1)
    {
        printf("No monitor is running\n");
        return;
    }
    
    if(monitor_exiting)
    {
        printf("Monitor is already exiting...\n");
        return;
    }
    monitor_exiting=1;
    kill(monitor_pid,SIGTERM);
    printf("Stopping monitor...\n");
}

void exit_monitor()
{
    if(monitor_pid!=-1)
    {
        fprintf(stderr,"Monitor is still running.\n");
        return;
    }
    printf("Exiting treasure hub\n");
    exit(0);
}

void list_hunts()
{

    if(monitor_pid==-1)
    {
        printf("Monitor not running\n");
        return;
    }
    kill(monitor_pid,SIGUSR1);
    read_monitor_output();
}

void list_treasures()
{
    if(monitor_pid==-1)
    {
        printf("Monitor not running\n");
        return;
    }
    char hunt_id[32];
    printf("Enter hunt id: ");
    fgets(hunt_id,sizeof(hunt_id),stdin);
    hunt_id[strcspn(hunt_id,"\n")]=0;

    kill(monitor_pid,SIGUSR2);
    send_data_to_monitor(hunt_id);
    send_data_to_monitor("\n");

    read_monitor_output();
}

void view_treasure()
{
    if(monitor_pid==-1)
    {
        printf("Monitor not running\n");
        return;
    }
    char hunt_id[32];
    char treasure_id[64];
    printf("Enter hunt ID: ");
    fgets(hunt_id,sizeof(hunt_id),stdin);
    hunt_id[strcspn(hunt_id,"\n")]=0;

    printf("Enter treasure ID: ");
    fgets(treasure_id,sizeof(treasure_id),stdin);
    treasure_id[strcspn(treasure_id,"\n")]=0;
    
    kill(monitor_pid,SIGINT);
    send_data_to_monitor(hunt_id);
    send_data_to_monitor(" ");
    send_data_to_monitor(treasure_id);
    send_data_to_monitor("\n");

    read_monitor_output();

}

void calculate_score()
{
    DIR *dir=opendir(".");
    if(!dir)
    {
        printf("Cannot open the current directory\n");
        return;
    }

    struct dirent *entry;
    while((entry=readdir(dir))!=NULL)
    {

        if(entry->d_type == DT_DIR && strcmp(entry->d_name,".") && strcmp(entry->d_name,".."))
        {
            int fd[2];
            if(pipe(fd)==-1)
            {
                perror("Pipe error\n");
                continue;
            }

            pid_t pid;
            pid=fork();
            if(pid==0)
            {
                close(fd[0]);
                dup2(fd[1],STDOUT_FILENO);
                close(fd[1]);
                execl("./calculate","./calculate",entry->d_name,NULL);
                perror("Error at execl\n");
                exit(1);
            }
            else
            {
                if(pid>0)
                {
                    close(fd[1]);
                    printf("Scores for %s:\n",entry->d_name);
                    char buffer[250];
                    ssize_t size;
                    while((size=read(fd[0],buffer,sizeof(buffer)-1))>0)
                    {
                        buffer[size]='\0';
                        printf("%s",buffer);
                    }
                    close(fd[0]);
                }
                else
                {
                    printf("Error at fork\n");
                }
            }
        }
    }
    closedir(dir);
}

int main()
{
    char cmd[128];
    while(1)
    {
        if(monitor_exiting && monitor_pid!=-1)
        {
            int status;
            pid_t result=waitpid(monitor_pid,&status,WNOHANG);
            if(result==monitor_pid)
            {
                printf("Monitor terminated with status %d\n",WEXITSTATUS(status));
                monitor_pid=-1;
                monitor_exiting=0;
            }
        }

        printf("treasure_hub > ");
        fflush(stdout);
        if(fgets(cmd,sizeof(cmd),stdin)==NULL)
        {
            break;
        }
        cmd[strcspn(cmd,"\n")]=0;

        if(monitor_exiting && strcmp(cmd,"exit")!=0 && strcmp(cmd,"start_monitor")!=0)
        {
            printf("Cannot accept commands until monitor has exited.\n");
            continue;
        }

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
        else if(strcmp(cmd,"calculate_score")==0)
        {
            calculate_score();
        }
        else
        {
            printf("Unknown command: %s\n",cmd);
        }
        
    }
    return 0;
}