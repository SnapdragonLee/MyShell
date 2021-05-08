//***************************************************************
//
//@file: Main.c
//
//@author: LD_ROOM
//
//@contact: lixiaolong8088888@gmail.com
//
//@brief: main function of MyShell++
//
//***************************************************************

#include "ShellCMD.h"

cmdStreamStruct *stream;

int main() {
    char line[LINE_MAX];
    int pid;

    mys_welcome();
    mys_shm_init();
    mys_init();

    while (1) {
        mys_prompt();
        mys_readLine(line);
        mys_analyzeCmd(line);

        if (0 != mys_builtinCmd()) {
            pid = fork();

            if (0 == pid) {
                mys_execute();
            }

            waitpid(pid, NULL, 0);
            sleep((unsigned int) 0.1);
        }

        mys_clearCmdStream(stream);
    }
} 

