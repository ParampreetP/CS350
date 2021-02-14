#include "parser.h"
#include <signal.h>
//Sources that helped me..
//
//https://www.geeksforgeeks.org/signals-c-set-2/
//https://stackoverflow.com/questions/50280498/how-to-only-kill-the-child-process-in-the-foreground
//
//

void printcmd(struct cmd *cmd)
{
    struct backcmd *bcmd = NULL;
    struct execcmd *ecmd = NULL;
    struct listcmd *lcmd = NULL;
    struct pipecmd *pcmd = NULL;
    struct redircmd *rcmd = NULL;

    int i = 0;
    
    if(cmd == NULL)
    {
        PANIC("NULL addr!");
        return;
    }
    

    switch(cmd->type){
        case EXEC:
            ecmd = (struct execcmd*)cmd;
            if(ecmd->argv[0] == 0)
            {
                goto printcmd_exit;
            }

            MSG("COMMAND: %s", ecmd->argv[0]);
            for (i = 1; i < MAXARGS; i++)
            {            
                if (ecmd->argv[i] != NULL)
                {
                    MSG(", arg-%d: %s", i, ecmd->argv[i]);
                }
            }
            MSG("\n");

            break;

        case REDIR:
            rcmd = (struct redircmd*)cmd;

            printcmd(rcmd->cmd);

            if (0 == rcmd->fd_to_close)
            {
                MSG("... input of the above command will be redirected from file \"%s\". \n", rcmd->file);
            }
            else if (1 == rcmd->fd_to_close)
            {
                MSG("... output of the above command will be redirected to file \"%s\". \n", rcmd->file);
            }
            else
            {
                PANIC("");
            }

            break;

        case LIST:
            lcmd = (struct listcmd*)cmd;

            printcmd(lcmd->left);
            MSG("\n\n");
            printcmd(lcmd->right);
            
            break;

        case PIPE:
            pcmd = (struct pipecmd*)cmd;

            printcmd(pcmd->left);
            MSG("... output of the above command will be redrecited to serve as the input of the following command ...\n");            
            printcmd(pcmd->right);

            break;

        case BACK:
            bcmd = (struct backcmd*)cmd;

            printcmd(bcmd->cmd);
            MSG("... the above command will be executed in background. \n");    

            break;


        default:
            PANIC("");
    
    }
    
    printcmd_exit:

    return;
}

void sigintHandler(int sig_num) //runs on the parent process
{
      printf("\nCtrl-C catched. But currently there is no foreground process running. \n"); 
      MSG("%s", SHELL_PROMPT);
   
  
} 
/*
void init_handler(){
    struct sigaction sa;

    sa.sa_handler = sigintHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    // If conditions for signal handling.
    // Also creates 2 signal handlers in memory for the SIGINT and SIGTSTP
    if(sigaction(SIGINT, &sa, NULL) == -1)
        printf("Couldn't catch SIGINT - Interrupt Signal\n");
    if(sigaction(SIGTSTP, &sa, NULL) == -1)
        printf("Couldn't catch SIGTSTP - Suspension Signal\n");
}
*/
int main(void)
{
    static char buf[1024];
    int fd;
    int status;
    setbuf(stdout, NULL);//blocks buffering
    //init_handler();
    signal(SIGINT, sigintHandler); 
    
    while(getcmd(buf, sizeof(buf)) >= 0)
    {
        struct cmd * command;
        command = parsecmd(buf);
        if(fork1() == 0){
	   runcmd(command); 
	
	}
	wait(&status);
    	if (WIFEXITED(status)) {
        	int es = WEXITSTATUS(status);
			if (es > 1) 
			printf("\nNon-zero exit code %d detected\n",es);
		
    	}
     }

    PANIC("getcmd error!\n");
    return 0;
}
