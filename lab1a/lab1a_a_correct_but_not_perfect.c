#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>


/******** variable **********/
int opt;
int opt_fault = 0;
int ret_fork;
int shell_flag = 0;
int debug_flag = 0;
int to_shell[2];
int to_termi[2];
char* which_shell = "/bin/bash";
char* buffer;
static struct option options[] = {
    {"shell",    2,  0,  's' },//TODO 2nd argument is???
    {"debug",    0,  0,  'g' },
    {0,          0,  0,   0  }
};
struct termios termios_p;
tcflag_t new_c_iflag;
tcflag_t new_c_oflag;
tcflag_t new_c_lflag;


void set_terminal(){
    //int err;
    
    //TODO what else need for error handler?
    if (tcgetattr(0,&termios_p) < 0) {
        fprintf(stderr, "Error in getting modes: %s\n", strerror(errno));
        //TODO restore the mode?
        exit(1);
    }
    else{
        //for restore purpose;
        new_c_iflag = termios_p.c_iflag;
        new_c_oflag = termios_p.c_oflag;
        new_c_lflag = termios_p.c_lflag;
        //modify the termios
        termios_p.c_iflag = ISTRIP;    /* only lower 7 bits    */
        termios_p.c_oflag = 0;        /* no processing    */
        termios_p.c_lflag = 0;        /* no processing    */
        
    }
    
    if ( tcsetattr(0,TCSANOW,&termios_p) < 0 ){
        fprintf(stderr, "Error in setting modes: %s\n", strerror(errno));
        //TODO restore the mode?
        exit(1);
    }
}

void restore_terminal(){
    //int err;
    termios_p.c_iflag = new_c_iflag;  /* only lower 7 bits */
    termios_p.c_oflag = new_c_oflag;  /* no processing     */
    termios_p.c_lflag = new_c_lflag;  /* no processing     */
    
    if ( tcsetattr(0,TCSANOW,&termios_p) < 0 ){
        fprintf(stderr, "Error in restoring the modes: %s\n", strerror(errno));
        exit(1);
    }
}

void signal_handler(int signum){
    //TODO what preperation to do??????
    if (signum == SIGPIPE){
        if (debug_flag) fprintf(stderr, "SIGPIPE received, begin to exit... %s\n", strerror(errno));
        exit(0);
    }
}

void read_from_shell(){};
void read_from_termi(){};


int main(int argc, char ** argv) {

    /*********** get argument *************/
    
    while ((opt = getopt_long(argc, argv, "", options, &opt_fault)) != -1){
        switch(opt){
            case 's':
                //TODO --shell command
                shell_flag = 1;
                if (optarg != NULL) which_shell = optarg;
                break;
                
            case 'g':
                //TODO --debug command
                debug_flag = 1;
                break;
            
            default:
                //TODO default option
                fprintf(stderr,"Error in finding unrecognized argument %c\n",(char)optopt);
                fprintf(stderr,"Correct Usage: ./lab1a --shell==SHELLNAME --debug\n");
                exit(1);
                break;
        }
        
    }
    
    if (debug_flag) fprintf(stderr,"Starting int main()......\n");
    if (debug_flag) fprintf(stderr,"Setting terminal mode......\n");
    set_terminal();
    
    /************ if --shell ****************/
    if (shell_flag){
        if (debug_flag) fprintf(stderr,"Shell flag reported...\n");
        signal(SIGPIPE, signal_handler);
        if (pipe(to_shell) < 0 ) fprintf(stderr,"Error in pipe(to_shell), %s\n", strerror(errno));
        if (pipe(to_termi) < 0 ) fprintf(stderr,"Error in pipe(to_termi), %s\n", strerror(errno));
            
        ret_fork = fork();
        if (ret_fork == 0){
            //TODO child process, how does pipe work?
            if (debug_flag) fprintf(stderr,"In child process, start io redrection...\n");
            
            if (close(to_shell[1]) < 0) fprintf(stderr,"Error in closing write end of to_Shell in child process, %s\n", strerror(errno));
            if (close(to_termi[0]) < 0) fprintf(stderr,"Error in closing read end of to_termi, %s\n", strerror(errno));
            
            if (close(0) < 0) fprintf(stderr,"Error in closing stdin in child process, %s\n", strerror(errno));
            if (dup(to_shell[0]) < 0) fprintf(stderr,"Error in dup read end of to_shell in child process, %s\n", strerror(errno));
            if (close(to_shell[0]) < 0) fprintf(stderr,"Error in closing read end of to_Shell in child process, %s\n", strerror(errno));
            if (close(1) < 0) fprintf(stderr,"Error in closing stdout, %s\n", strerror(errno));
            if (dup(to_termi[1]) < 0) fprintf(stderr,"Error in dup write end of to_termi in child process, %s\n", strerror(errno));
            if (close(2) < 0) fprintf(stderr,"Error in closing stderr, %s\n", strerror(errno));
            if (dup(to_termi[1]) < 0) fprintf(stderr,"Error in dup write end of to_termi in child process, %s\n", strerror(errno));
            if (close(to_termi[1]) < 0) fprintf(stderr,"Error in closing write end of to_termi in child process, %s\n", strerror(errno));
            
            if (debug_flag) fprintf(stderr,"Finishing child process, calling execlp(), with shell name: %s...", which_shell);
            if (execlp(which_shell,which_shell,(char*)NULL) < 0)//TODO check execlp
                fprintf(stderr,"Error execlp in the child process, exiting with code 1, %s\n", strerror(errno));
            exit(1);

        }
        else if (ret_fork > 0){
            int exit_flag;
            int wait_status;
            char* buffer_to_shell = (char*) malloc(sizeof(char)*256);
            char* buffer_to_termi = (char*) malloc(sizeof(char)*256);
            struct pollfd fds[] = {
                {0, POLLIN|POLLHUP|POLLERR, 0},
                {to_termi[0], POLLIN|POLLHUP|POLLERR ,0}
            };
            
            //TODO parent process;
            if (debug_flag) fprintf(stderr,"Starting parent process.....");
            if (debug_flag) fprintf(stderr,"In parent process, start io redrection...\n");
            
            if (close(to_shell[0]) < 0) fprintf(stderr,"Error in closing write end of to_Shell in parent process, %s\n", strerror(errno));
            if (close(to_termi[1]) < 0) fprintf(stderr,"Error in closing read end of to_termi in parent process, %s\n", strerror(errno));

            if (debug_flag) fprintf(stderr,"Begin to process poll(), into the while(1) loop...\n");
            while(1){
                exit_flag = 0;
                if (poll(fds, 2, -1) == -1) {
                    fprintf(stderr,"Error: poll() failed in parent process %s\n", strerror(errno));
                    exit(1);
                }
                //TODO why in the while loop?
                if (fds[0].revents & POLLIN) {
                    //TODO forward to stdout, to_shell [1]
                    if (debug_flag) fprintf(stderr,"\nFind read to shell: fds[0].revents & POLLIN...\n");
                    ssize_t read_to_shell = read(0, buffer_to_shell, sizeof(buffer_to_shell));
                    if (read_to_shell < 0) fprintf(stderr,"fds[0]: error in reading before forward to stdout and shell, %s", strerror(errno));
                    //TODO exit???
                    else {
                        for (int i = 0 ; i < read_to_shell; i++){
                            char a_char = buffer_to_shell[i];
                            /*if (debug_flag)
                                fprintf(stderr,"Parent process: fds[0] writing char: %d, %c", i, a_char);*/
                            if (a_char == '\r' ||a_char == '\n' ){
                                if (write(1, "\r\n", 2) < 0)
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                                if (write(to_shell[1], "\n", 1) < 0)
                                    fprintf(stderr,"Error in writing to shell, %s", strerror(errno));
                            }
                            else if (a_char == 0x04){
                                if (write(1, "^D", 2) < 0)
                                    fprintf(stderr,"Error in writing eof to stdout, %s", strerror(errno));
                                if (debug_flag) fprintf(stderr, "^D received, loop breaking\n");
                                //TODOxiao pan mei you
                                exit_flag = 1;
                                //TODO close file descripter?
                                
                            }
                            else if (a_char == 0x03){
                                if (write(1, "^C", 2) < 0)
                                    fprintf(stderr,"Error in writing interrupt to stdout, %s", strerror(errno));
                                //TODO kill? why need kill? which signal???
                                if (kill(ret_fork, SIGINT) < 0)
                                {
                                    fprintf(stderr, "Error in sending ginal SIGINT to shell: %s\n", strerror(errno));
                                    exit(1);
                                }
                                
                            }
                            else{
                                if (write(1, &a_char, 1) < 0)
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                                if (write(to_shell[1], &a_char, 1) < 0)
                                    fprintf(stderr,"Error in writing to shell, %s", strerror(errno));
                            }
                        }
                    }
                }
                if (fds[1].revents & POLLIN) {
                    //TODO forward to stdout.
                    //TODO new buffer
                    ssize_t read_to_termi = read(to_termi[0], buffer_to_termi, sizeof(buffer_to_termi));
                    if (debug_flag) fprintf(stderr,"\nFind read to shell: fds[1].revents & POLLIN...\n");
                    if (read_to_termi < 0) fprintf(stderr,"fds[1]: error in reading before forward to stdout, %s", strerror(errno));
                    else {
                        for (int i = 0 ; i < read_to_termi; i++){
                            char b_char = buffer_to_termi[i];
                            /*if (debug_flag)
                                fprintf(stderr,"Parent process: fds[1] writing char: %d, %c", i, b_char);*/
                            if (b_char == '\r' ||b_char == '\n' ){
                                if (write(1, "\r\n", 2) < 0)
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                            }
                            else if (b_char == 0x04){
                                if (write(1, "^D", 2) < 0)
                                    fprintf(stderr,"Error in writing eof to stdout, %s", strerror(errno));
                                if (debug_flag) fprintf(stderr, "^D received, looop breaking");
                                exit_flag = 1;
                                //TODO close file descripter?
                            }
                            else{
                                if (write(1, &b_char, 1) < 0)
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                            }
                        }
                    }
                }
                if (fds[0].revents & (POLLHUP | POLLERR))  {
                    if (debug_flag) fprintf(stderr,"Finish reading from stdin...\n");
                    exit_flag = 1;
                }
                if (fds[1].revents & (POLLHUP | POLLERR))  {
                    if (debug_flag) fprintf(stderr,"Finish reading from shell...\n");
                    exit_flag = 1;
                }
                if (exit_flag) {
                    if (debug_flag) fprintf(stderr, "Exit flag received, waiting for child and exit...");
                    if (close(to_shell[1]) < 0) fprintf(stderr, "Parent process: rror in closing to_shell[1], %s\n",strerror(errno));
                    if (waitpid(ret_fork, &wait_status, 0) < 0)
                        fprintf(stderr, "Error in waiting for child process to finish, %s\n",strerror(errno));
                    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WIFSIGNALED(wait_status), WEXITSTATUS(wait_status));//TODO is this method correct?
                    break;
                }
            }

        }
        /***fail to fork()****/
        else{
            fprintf(stderr, "Error: Fork() failed. %s\n", strerror(errno));
            exit(1);
        }
    }
    
    /*********** no --shell ************/
    else
    {
        buffer = (char*) malloc(sizeof(char)*256);
        ssize_t read_num = 0;
        int break_flag_no_shell = 0;
        //TODO read more than one
        if (debug_flag) fprintf(stderr, "Starting non-shell mode\n");
        while (1)
        {
            read_num = read(0, buffer, sizeof(buffer));
            if ( read_num < 0) {
                fprintf(stderr, "Error: fail to read from stdin, %s\n", strerror(errno));
                exit(1);
            }
            for (int i = 0; i < read_num; i++){
                char c_char = buffer[i];
                if (debug_flag)
                    fprintf(stderr,"Non-shell mode, writing char: %d, %c. \n", i, c_char);
                if (c_char == '\r' ||c_char == '\n' ){
                    if (write(0, "\r\n", 2) < 0)
                        fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                }
                else if (c_char == 0x04){
                    if (write(0, "^D", 2) < 0)
                        fprintf(stderr,"Error in writing eof to stdout, %s", strerror(errno));
                    if (debug_flag) fprintf(stderr, "^D received, looop breaking");
                    break_flag_no_shell = 1;
                    //TODO close file descripter?
                }
                
                //TODO no need for this one?
                else if (c_char == 0x03){
                    if (write(0, "^C", 2) < 0)
                        fprintf(stderr,"Error in writing interrupt to stdout, %s", strerror(errno));
                    //TODO kill?
                }
                else{
                    if (write(0, &c_char, 1) < 0)
                        fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                }
            }
            if (break_flag_no_shell) break;
        }
    }

    /**********finishing up*********/
    restore_terminal();
    free(buffer);
    if (debug_flag) fprintf(stderr, "Reach the end of the program.\n");
    exit(0);
}

