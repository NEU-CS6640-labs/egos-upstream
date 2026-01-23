/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: a simple shell
 *
 * Updated by OSI 26spring staff
 */

#include "app.h"
#include <string.h>
#include <stdlib.h>

char* parse_batch_cmds(char *buf) {
    static char cmds[10][256] = {0};
    static int cmd_idx = -1;
    static int max_idx = -1;

    if (cmd_idx == -1) {  // -1: no prior cmds
        if(buf[0] != '(' || buf[strnlen(buf, 256)-1] != ')' ) {
            strncpy(cmds[0], buf, 256);
            cmd_idx = 0;
            max_idx = 0;
        } else {
            char *copy = strdup(buf);
            copy[strnlen(copy, 256)-1] = '\0'; // remove ')'

            char *cmd; int num_cmd = 0;
            for (cmd = strtok(&copy[1], ";"); cmd;
                 cmd = strtok(NULL, ";"))
            {
                strncpy(cmds[num_cmd++], cmd, 256);
                if (num_cmd > 10) {
                    INFO("sys_shell: too many cmds in a batched cmd");
                    free(copy);
                    return NULL;
                }
            }
            free(copy);

            // update static vars
            cmd_idx = 0; // start with 0
            max_idx = num_cmd - 1; // final cmd
        }
    }

    ASSERT(cmd_idx >= 0 && cmd_idx <= max_idx+1, "");
    int cur = cmd_idx;
    if (cmd_idx <= max_idx) { // more cmds to go
        cmd_idx++;
        return cmds[cur];
    } else { // finishing this batch
        cmd_idx = -1;
        max_idx = -1;
        return NULL;
    }
}


int parse_request(char* buf, struct proc_request* req) {
    uint idx = 0, nargs = 0;
    memset(req->argv, 0, CMD_NARGS * CMD_ARG_LEN);

    for (uint i = 0; i < strlen(buf); i++)
        if (buf[i] != ' ') {
            req->argv[nargs][idx] = buf[i];
            if (++idx >= CMD_ARG_LEN) return -1;
        } else if (idx != 0) {
            idx = req->argv[nargs][idx] = 0;
            if (++nargs >= CMD_NARGS) return -1;
        }
    req->argc = idx ? nargs + 1 : nargs;
    return 0;
}

int main() {
    CRITICAL("Welcome to the egos-2k+ shell!");

    char buf[TERM_BUF_SIZE] = "cd"; /* Enter the home directory first. */
    while (1) {
        char *nextcmd;
        while ( (nextcmd = parse_batch_cmds(buf)) != NULL ) {
            struct proc_request req;
            struct proc_reply reply;

            if (strcmp(nextcmd, "coresinfo") == 0) {
                // TODO
            } else if (strcmp(nextcmd, "killall") == 0) {
                req.type = PROC_KILLALL;
                grass->sys_send(GPID_PROCESS, (void*)&req, sizeof(req));
            } else if (strcmp(nextcmd, "pwd") == 0) {
                printf("%s\n\r", workdir);
            } else {
                req.type = PROC_SPAWN;
                if (0 != parse_request(nextcmd, &req)) {
                    INFO("sys_shell: too many arguments or argument too long");
                } else {
                    grass->sys_send(GPID_PROCESS, (void*)&req, sizeof(req));
                    grass->sys_recv(GPID_PROCESS, NULL, (void*)&reply,
                            sizeof(reply));

                    if (reply.type != CMD_OK)
                        INFO("sys_shell: command %s not found", req.argv[0]);
                    else if (req.argv[req.argc - 1][0] != '&')
                        /* Wait for the foreground command to terminate. */
                        grass->sys_recv(GPID_PROCESS, NULL, (void*)&reply,
                                sizeof(reply));
                }
            }
        }

        do {
            printf("\x1B[1;32m➜ \x1B[1;36m%s\x1B[1;0m %% ", workdir);
        } while (term_read(buf, TERM_BUF_SIZE) == 0);
    }
}
