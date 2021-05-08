//***************************************************************
//
//@file: ShellCMD.h
//
//@author: LD_ROOM
//
//@contact: lixiaolong8088888@gmail.com
//
//@brief: header of MyShell++
//
//***************************************************************

#ifndef __SHELLCMD_H__

#define __SHELLCMD_H__

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>

/* The size of share memory */
#define SHM_MEM_SIZE 2048 * 16

/* The size of the cmdStream */
#define STREAM_SZ 8

/* The max_length of a simple cmd ( = the amount of parameters + 1 ) */
#define CMD_LENGTH 32

/* The max_length of a parameter */
#define PARA_MAX 64

/* The max length of a line from input */
#define LINE_MAX 1024 * 8


/* The following is a simple command structure */
typedef struct CMD_STRUCT {
    char *cmd[CMD_LENGTH];
    char cmdStr[CMD_LENGTH * PARA_MAX];
    char nextSign; // '|\0\0', '>\0\0', '<\0\0', '>>\0'
} cmdStruct;

/* Command stream including N complete commands and pointer */
typedef struct CMD_STREAM_STRUCT {
    cmdStruct cmdStream[STREAM_SZ];
    int cmdPtr;
} cmdStreamStruct;

/* One command line */
extern cmdStreamStruct *stream;


/* One command line */
int mys_welcome(void);

/* One command line */
int mys_shm_init(void);

/* Initialize prompt */
int mys_init(void);

/* Change work directory */
int mys_cd(void);

/* Print help information */
int mys_help(void);

/* Exit MyShell++ */
int mys_exit(void);

/* Print prompt */
int mys_prompt(void);

/* Read a line from stdin */
int mys_readLine(char *line);

/* Copy a substring from srcStr to resultStr */
int mys_subStr(char *resultStr, char *str, int origin, int end);

/* Split a string according to splitChar */
int mys_splitStr(char *resultArr[], char *str, char *split);

/* Analyse and format the command */
int mys_analyzeCmd(char *line);

/* Execute internal command by define*/
int mys_builtinCmd(void);

/* Execute external command with recursion.
 * What’s more , builtin commands don’t support pipe and redirection.*/
int mys_execute(void);


/* Clear current cmdStruct */
int mys_clearCmdStream(cmdStreamStruct *stream);

/* Print 'command () not found' messages */
int mys_commandFound(int localPtr);

#endif
