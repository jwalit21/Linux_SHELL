/* 
Author: JWALIT C SHAH
Date-of creation-completion: 31/08/2020 3:30PM-11:30PM
This code includes all necessary cooments rneeding for underatanding and mentioned certain EXCEPTIONs in this program 
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char ip, command1[1000], command2[1000], command[1000], str[1000], buff[1000], comm[1000], option[1000], pipecomm[1000], prev_command_cache[1000] = {'\0'}, *ptrp, *ptro;
    int i, k, j, option_pipe, fd, pid, pid_inner, p, op, com, flagpipe = 0, flagop = 0, iscat = 0, pointer = -1;
    while (1)
    {
        int pipefd[2];
        char root[100] = "/bin/"; // root for all commands in linux
        int status, status_inner;

        ptrp = NULL;
        ptro = NULL; // initializing all variables to initial value
        iscat = 0;
        flagop = 0;
        flagpipe = 0, k = 0;

        for (i = 0; i < 1000; i++) // for initializing all valid arrays to null
        {
            command[i] = '\0';
            command1[i] = '\0';
            command2[i] = '\0';
            str[i] = '\0';
            buff[i] = '\0';
            comm[i] = '\0';
            option[i] = '\0';
            pipecomm[i] = '\0';
        }
        printf("jwalit@ : ");

        scanf("%c", &ip); // if at promt, enter is provided, then again output a promt
        if (ip == '\n')
        {
            continue;
        }
        command[0] = ip;
        for (i = 1; i < 1000; i++) // continuing to take command
        {
            scanf("%c", &ip);
            if (ip == '\n') // take command untill \n is there
            {
                command[i] = '\0';
                break;
            }
            command[i] = ip;
        }

        if (strcmp(command, "up") == 0) // CUSTOM COMMAND "up" will cache a last command and executes it if it is available in cache;
        {
            if (pointer == -1) // if cache is already empty
                continue;
            if (pointer == 0) // previously executed command is cached already
            {
                printf("jwalit@ : %s\n", prev_command_cache);
                strcpy(command, prev_command_cache);
            }
        }
        else if (strcmp(command, "swipeout") == 0) // CUSTOM COMMMAND "swipeout" for clearing the cache
        {
            printf("Previous command Cache cleared successfully\n");
            pointer = -1;
            continue;
        }
        else if(strcmp(command,"exit")==0)
        {
            printf("Thank you..!!\n");
            break;
        }
        else
        {
            pointer = 0;
            strcpy(prev_command_cache, command); // setting the command cache to current command
        }

        strcpy(command1, command); // for initializing all command sample array
        strcpy(command2, command);

        ptro = strtok(command, " ");     // splitting the whole command intp parts
        if (strcmp(ptro, command1) == 0) // after splitting by " " ; even if command is same then there might be a pipe operation or it may don't have |
        {
            ptrp = strtok(command1, "|");    // splitting two commands fro piping
            if (strcmp(ptrp, command2) != 0) // if we cant find any pipe then it is sure that it is only single command
            {
                flagpipe = 1;       // indicating that only pipe is present (there is no previous option before pipe)
                strcpy(comm, ptrp); // copy left most command
                ptrp = strtok(NULL, " ");
                if (ptrp == NULL || strlen(ptrp) == 0) // if after | there is nothing written there
                {
                    printf("please enter a valid pipe command after pipe\n");
                    continue;
                }
                else
                    strcpy(pipecomm, ptrp); // copy pipe command
            }
            else                    // only a single command
                strcpy(comm, ptrp); // copy leftmost command
        }
        else // now command has option attatched with it, it may possible that after option there would be a pipe(|)
        {
            flagop = 1;               // indicating that option is present
            strcpy(comm, ptro);       // copy leftmost command
            ptro = strtok(NULL, " "); // gets next option

            if (strcmp(ptro, ">") == 0) // if ">" as a option then, we have rediection to file then.
            {
                iscat = 1;                // indicating that redirection is present in the command
                ptro = strtok(NULL, " "); // taking another option for redirection which is a file
            }
            option_pipe = 0;
            for (i = 0; i < strlen(ptro); i++)
            {
                if (ptro[i] == '|')
                {
                    flagpipe = 1;                   // indicating that a pipe is present in command
                    option_pipe = 1;                // indicating that first option is present and then we have a pipe
                    strcpy(pipecomm, ptro + i + 1); // copy piped command
                    break;
                }
                option[i] = ptro[i]; // itrating threw all and untill pipe comes or till length we store in option
                option[i + 1] = '\0';
            }
        }

        strcat(root, comm); // have a base path for commands
        fd = open(root, O_RDONLY);
        if (fd != -1) // check if it is a valid command undelying in the "/bin/"
        {

            pid = fork(); // create child process for execution of every command
            if (pid == -1)
                printf("Internal fork error\n");
            else if (pid > 0)
                wait(&status); //wait untill child finishes the command
            else
            {
                if (flagop == 1 && flagpipe == 0) // only option is there, no piping
                {
                    if (iscat == 1) // in option we are having a ">" (redirection)
                    {
                        fd = open(option, O_WRONLY); // opens or creates a file , provided in the option
                        if (fd == -1)
                        {
                            printf("File open error\n");
                            continue;
                        }
                        else
                        {
                            dup2(fd, 1);                               // putting the inputed value to the file which program read for saving to the file
                            if (execl(root, comm, (char *)NULL) == -1) // cat to a file
                                printf("Internal exec error\n");
                        }
                    }
                    else // we dont have redirection , only pure optioning
                    {
                        if (execl(root, comm, option, (char *)NULL) == -1) // executes command with the option; LIMITATION; ONLY ONE OPTION IS ALLOWED HERE
                            printf("Internal exec error\n");
                    }
                }
                else if (flagpipe == 1) // it has pipe ; it may also have a option too
                {
                    p = pipe(pipefd);
                    if (p == -1)
                    {
                        printf("Internal pipe error\n");
                        continue;
                    }

                    pid_inner = fork(); // forking for two command to get runned
                    if (pid_inner == -1)
                        printf("Internal fork error\n");
                    else if (pid_inner > 0) // piped command execution
                    {
                        wait(&status_inner); // wait till left most is finished
                        close(pipefd[1]);
                        dup2(pipefd[0], 0); // for read input from pipe
                        char root1[100] = "/bin/";
                        strcat(root1, pipecomm); // make a  base path of piped command
                        int fd2 = open(root1, O_RDONLY);
                        if (fd2 == -1) // check if it is a valid command undelying in the "/bin/"
                        {
                            printf("please enter a valid piped command\n");
                            continue;
                        }
                        if (execl(root1, pipecomm, (char *)NULL) == -1) // execute a piped command (WITHOUT ANY OPTION AND PIPELINING FURTHER)
                            printf("Internal exec error\n");
                    }
                    else
                    {
                        close(pipefd[0]);
                        dup2(pipefd[1], 1);   //to put output to pipe
                        if (option_pipe == 1) // there might be aption in leftmost piped command (REDIRECTION WILL NOT BE THERE)
                        {

                            if (execl(root, comm, option, (char *)NULL) == -1) // execute command with that option
                                printf("Internal exec error\n");
                        }
                        else // command without any option
                        {
                            if (execl(root, comm, (char *)NULL) == -1) // execute command which is without option
                                printf("Internal exec error\n");
                        }
                    }
                }
                else                                           // pure command is there without pipe and without option
                    if (execl(root, comm, (char *)NULL) == -1) // execute command
                    printf("Internal exec error\n");
            }
        }
        else // command's base path is not valid or not available in "/bin/"
        {
            printf("please enter a valid command\n");
            continue;
        }
    }
    return 0;
}