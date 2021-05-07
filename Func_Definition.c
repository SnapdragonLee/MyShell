//***************************************************************
//
//@file: Func_Definition.c
//
//@author: LD_ROOM
//
//@contact: lixiaolong8088888@gmail.com
//
//@brief: implementation of all API functions
//
//***************************************************************

#include "ShellCMD.h"

/* Current directory of MyShell++ */
char dirPrompt[50];

/* List of builtIn command */
char *builtInCmd[] = {"cd", "help", "exit"};

/* Shared memory id */
int shm_id;

/* Shared memory buffer */
char *shm_buff;

int mys_welcome(void) {
    printf("Welcome to use MyShell++!!!\n");

    return 0;
}

int mys_shm_init(void) {
    int key;
    key = ftok("Myshell++", 1);
    shm_id = shmget(key, SHM_MEM_SIZE, IPC_CREAT | 0666);
    shm_buff = (char *) shmat(shm_id, 0, 0);
    stream = (cmdStreamStruct *) shm_buff;

    return 0;
}


int mys_init(void) { // finish
    char *dirArr[30];
    char dir[400];
    int i;

    stream->cmdPtr = 0;
    getcwd(dir, 400);
    mys_splitStr(dirArr, dir, "/");

    //find current directory string
    for (i = 0;; i++) {
        if (NULL == dirArr[i])
            break;
    }

    strcpy(dirPrompt, dirArr[i - 1]);

    return 0;
}


int mys_cd(void) { // finish
    int i;
    char *dirArr[30];
    char dir[400];

    if (0 != chdir(stream->cmdStream[0].cmd[1])) {
        perror("cd");
        return -1;
    } else {
        getcwd(dir, 400);
        mys_splitStr(dirArr, dir, "/");

        for (i = 0; NULL != dirArr[i]; i++); // Find latest directory
        strcpy(dirPrompt, dirArr[i - 1]);
    }

    return 0;
}


int mys_help(void) { // updating
    printf("\n"
           "***************************************************************\n"
           "Latest Version Detail: version 1.0.1\n"
           "Add pipe support from external command\n\n"
           "Previous Version Detail: version 1.0.04\n"
           "Add commands from internal or external\n"
           "Internal Command included: cd, help, exit\n"
           "***************************************************************\n\n"
           "The shell commands are defined externally.  Type `help' to see this info.\n\n"
           "Details of MyShell++: \n"
           "MyShell++, version 1.0.1\n"
           "Last Update: 2021.5.8\n"
           "Author: LD_ROOM\n"
           "Contact: 13671390321\n"
           "Email: lixiaolong8088888@gmail.com\n\n");

    return 0;
}


int mys_exit(void) { // finish
    shmdt(shm_buff);
    shmctl(shm_id, IPC_RMID, 0);
    printf("logout from MyShell++ ...\n");
    exit(EXIT_SUCCESS);
}


int mys_prompt() { // finish
    printf("\033[0;36m%s\033[0m", dirPrompt);
    printf("\033[0;31m$\033[0m");

    return 0;
}


int mys_readLine(char *line) { // finish
    int pos = 0;
    char c;

    while (1) {
        c = (char) getchar();
        line[pos++] = c;

        if (c == '\n') {
            break;
        }
    }

    return 0;
}


int mys_subStr(char *resultStr, char *str, int origin, int end) { // finish
    int length = end - origin + 1;

    if (end < origin) {
        printf("Error at mys_subStr!\n");
        return -1;
    }

    memcpy(resultStr, str + origin, length);
    resultStr[length] = '\0';
    //printf("%s", resultStr);

    return 0;
}


int mys_splitStr(char *resultArr[], char *str, char *split) {
    int position = 0;
    char *token;

    token = strtok(str, split);
    while (token != NULL) {
        resultArr[position] = token;
        position++;
        token = strtok(NULL, split);
    }
    resultArr[position] = NULL;

    return 0;
}


int mys_analyzeCmd(char *line) {
    int i, j = 0;
    int lastPosition = 0;
    cmdStruct *curCommand;

    for (i = 0; '\n' != line[i]; i++) {
        if ('|' == line[i] || '<' == line[i] || '>' == line[i]) {
            curCommand = &stream->cmdStream[j];
            mys_subStr(curCommand->cmdStr, line, lastPosition, i - 1);
            mys_splitStr(curCommand->cmd, curCommand->cmdStr, " ");
            curCommand->nextSign = line[i];
            lastPosition = i + 1;
            j++;
        }
    }

    curCommand = &stream->cmdStream[j];
    mys_subStr(curCommand->cmdStr, line, lastPosition, i - 1);
    mys_splitStr(curCommand->cmd, curCommand->cmdStr, " ");

    return 0;
}

int mys_builtinCmd(void) {
    int i;

    for (i = 0; i < 3; i++) {
        if (0 == strcmp(builtInCmd[i], stream->cmdStream[0].cmd[0])) {
            break;
        }
    }

    switch (i) {
        case 0:
            mys_cd();
            break;

        case 1:
            mys_help();
            break;

        case 2:
            mys_exit();
            break;

            /*case 3:
                break;

            case 4:
                break;

            case 5:
                break;*/

        default:
            return -1;
    }

    return 0;
}


int fd[2];

int mys_execute() {
    int pid, localPtr;

    if ('|' == stream->cmdStream[stream->cmdPtr].nextSign) {
        localPtr = stream->cmdPtr;
        pipe(fd);

        pid = fork();

        if (0 == pid)//execute next command in child process
        {
            stream->cmdPtr++;

            dup2(fd[0], 0);//redirect standard input to pipe(read)
            close(fd[0]);
            close(fd[1]);

            mys_execute();
            exit(EXIT_SUCCESS);
        } else//execute current command in current process
        {
            signal(SIGCHLD, SIG_IGN);

            dup2(fd[1], 1);//redirect standard output to pipe(write)
            close(fd[0]);
            close(fd[1]);

            mys_commandFound(localPtr);

            exit(EXIT_SUCCESS);
        }
    } else if ('<' == stream->cmdStream[stream->cmdPtr].nextSign) {
        localPtr = stream->cmdPtr;
        char fileName[20];

        strcpy(fileName, stream->cmdStream[localPtr + 1].cmd[0]);
        freopen(fileName, "r", stdin);//redirect stdin to fileName

        if ('>' == stream->cmdStream[stream->cmdPtr + 1].nextSign) {
            strcpy(fileName, stream->cmdStream[localPtr + 2].cmd[0]);
            freopen(fileName, "w", stdout);//redirect stdout to fileName
        }
        if ('|' == stream->cmdStream[stream->cmdPtr + 1].nextSign) {
            pipe(fd);
            pid = fork();
            if (0 == pid)//execute next command in child process
            {
                stream->cmdPtr += 2;

                dup2(fd[0], 0);//redirect standard input to pipe(read)
                close(fd[0]);
                close(fd[1]);

                mys_execute();
                exit(EXIT_SUCCESS);
            } else//execute current command in current process
            {

                signal(SIGCHLD, SIG_IGN);

                dup2(fd[1], 1);//redirect stdout to pipe(write)
                close(fd[0]);
                close(fd[1]);

                mys_commandFound(localPtr);
            }

        }
        mys_commandFound(localPtr);

        exit(EXIT_SUCCESS);

    } else if ('>' == stream->cmdStream[stream->cmdPtr].nextSign) {
        localPtr = stream->cmdPtr;
        char fileName[20];

        strcpy(fileName, stream->cmdStream[localPtr + 1].cmd[0]);
        freopen(fileName, "w", stdout);//redirect stdout to fileName
        mys_commandFound(localPtr);

        exit(EXIT_SUCCESS);
    } else {
        localPtr = stream->cmdPtr;
        mys_commandFound(localPtr);
        exit(EXIT_SUCCESS);
    }
}

int mys_clearCmdStream(cmdStreamStruct *streamField) {
    memset(streamField, 0, sizeof(cmdStreamStruct));

    return 0;
}

int mys_commandFound(int localPtr) {
    cmdStruct curCommand = stream->cmdStream[localPtr];
    if (0 != execvp(curCommand.cmd[0], curCommand.cmd)) {
        printf("Command '%s' not found\n", curCommand.cmdStr);
    }

    return 0;
}
