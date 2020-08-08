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
int free_flag = 0;
int wait_status;
int to_shell[2];
int to_termi[2];
char* which_shell = "/bin/bash";
char* buffer;
char* buffer_to_shell;
char* buffer_to_termi;
static struct option options[] = {
    {"shell",    1,  0,  's' },
    {"debug",    0,  0,  'g' },
    {0,          0,  0,   0  }
};
struct termios termios_p;
tcflag_t new_c_iflag;
tcflag_t new_c_oflag;
tcflag_t new_c_lflag;

void before_exit(){
    if (shell_flag) {
        if (waitpid(ret_fork, &wait_status, 0) < 0)
            fprintf(stderr, "Error in waiting for child process to finish, %s\n",strerror(errno));
        fprintf(stderr, "\r\nSHELL EXIT SIGNAL=%d STATUS=%d\r\n", WIFSIGNALED(wait_status), WEXITSTATUS(wait_status));
        if (free_flag){
            free(buffer_to_termi);
            free(buffer_to_shell);
        }
        free(which_shell);
    }
    else free(buffer);
    
    termios_p.c_iflag = new_c_iflag;
    termios_p.c_oflag = new_c_oflag;
    termios_p.c_lflag = new_c_lflag;
    
    if ( tcsetattr(0,TCSANOW,&termios_p) < 0 ){
        fprintf(stderr, "Error in restoring the modes: %s\n", strerror(errno));
        exit(1);
    }
    
    if (debug_flag) fprintf(stderr, "Reach the end of the program.\r\n");
}

void set_terminal(){
    
    if (tcgetattr(0,&termios_p) < 0) {
        fprintf(stderr, "Error in getting modes: %s\n", strerror(errno));
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
        exit(1);
    }
    
    if (atexit(before_exit) < 0) {
        fprintf(stderr, "Error: cannot set exit function\n");
        exit(1);
    }
    else {
        if (debug_flag) fprintf(stderr, "Successfully set atexit()...\r\n");
    }
}

void signal_handler(int signum){
    if (signum == SIGPIPE){
        if (debug_flag) fprintf(stderr, "SIGPIPE received, begin to exit... %s\n", strerror(errno));
        exit(0);
    }
}

int main(int argc, char ** argv) {

    /*********** get argument *************/
    
    while ((opt = getopt_long(argc, argv, "", options, &opt_fault)) != -1){
        switch(opt){
            case 'g':
                debug_flag = 1;
                break;
                
            case 's':
                shell_flag = 1;
                which_shell = malloc((strlen(optarg)+1)*sizeof(char));
                memcpy(which_shell,optarg,(strlen(optarg)+1)*sizeof(char));
                break;
            
            default:
                fprintf(stderr,"Error in finding urrecognized argument or missing required input for --shell \n");
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
        if (debug_flag) fprintf(stderr,"Shell flag reported...\r\n");
        signal(SIGPIPE, signal_handler);
        if (pipe(to_shell) < 0 ) {
            fprintf(stderr,"Error in pipe(to_shell), %s\n", strerror(errno));
            exit(1);
        }
    
        if (pipe(to_termi) < 0 ) {
            fprintf(stderr,"Error in pipe(to_termi), %s\n", strerror(errno));
            exit(1);
        }
            
        ret_fork = fork();
        if (ret_fork == 0){
            if (debug_flag) fprintf(stderr,"\rIn child process, start io redrection...\r\n");
            
            if (close(to_shell[1]) < 0) {
                fprintf(stderr,"Error in closing write end of to_Shell in child process, %s\n", strerror(errno));
                exit(1);
            }
            if (close(to_termi[0]) < 0) {
                fprintf(stderr,"Error in closing read end of to_termi, %s\n", strerror(errno));
                exit(1);
            }
            
            if (close(0) < 0) {
                fprintf(stderr,"Error in closing stdin in child process, %s\n", strerror(errno));
                exit(1);
            }
            if (dup(to_shell[0]) < 0) {
                fprintf(stderr,"Error in dup read end of to_shell in child process, %s\n", strerror(errno));
                exit(1);
            }
            if (close(to_shell[0]) < 0) {
                fprintf(stderr,"Error in closing read end of to_Shell in child process, %s\n", strerror(errno));
                exit(1);
                
            }

            if (close(1) < 0) {
                fprintf(stderr,"Error in closing stdout, %s\n", strerror(errno));
                exit(1);
                
            }
            
            if (dup(to_termi[1]) < 0) {
                fprintf(stderr,"Error in dup write end of to_termi in child process, %s\n", strerror(errno));
                exit(1);
            }
            if (close(2) < 0) {
                fprintf(stderr,"Error in closing stderr, %s\n", strerror(errno));
                exit(1);
            }
            if (dup(to_termi[1]) < 0) {
                fprintf(stderr,"Error in dup write end of to_termi in child process, %s\n", strerror(errno));
                exit(1);
            }
            if (close(to_termi[1]) < 0) {
                fprintf(stderr,"Error in closing write end of to_termi in child process, %s\n", strerror(errno));
                exit(1);
            }
            
            if (debug_flag) fprintf(stderr,"Finishing child process, calling execlp(), with shell name: %s...\r\n", which_shell);
            if (execlp(which_shell,which_shell,(char*)NULL) < 0)
                fprintf(stderr,"Error execlp in the child process: %s\n", strerror(errno));
            exit(1);
        }
        
        else if (ret_fork > 0){
            int exit_flag;
            buffer_to_shell = (char*) malloc(sizeof(char)*256);
            buffer_to_termi = (char*) malloc(sizeof(char)*256);
            free_flag = 1;
            struct pollfd fds[] = {
                {0, POLLIN|POLLHUP|POLLERR, 0},
                {to_termi[0], POLLIN|POLLHUP|POLLERR ,0}
            };
            
            if (debug_flag) fprintf(stderr,"In parent process, start io redrection...\r\n");
            
            if (close(to_shell[0]) < 0) {
                fprintf(stderr,"Error in closing write end of to_Shell in parent process, %s\n", strerror(errno));
                exit(1);
            }
            if (close(to_termi[1]) < 0) {
                fprintf(stderr,"Error in closing read end of to_termi in parent process, %s\n", strerror(errno));
                exit(1);
            }
            
            if (debug_flag) fprintf(stderr,"Begin to process poll(), into the while(1) loop...\r\n");
            while(1){
                exit_flag = 0;
                if (poll(fds, 2, -1) == -1) {
                    fprintf(stderr,"Error: poll() failed in parent process %s\n", strerror(errno));
                    exit(1);
                }
                if (fds[0].revents & POLLIN) {
                    ssize_t read_to_shell = read(0, buffer_to_shell, sizeof(buffer_to_shell));
                    if (read_to_shell < 0) {
                        fprintf(stderr,"fds[0]: error in reading before forward to stdout and shell, %s", strerror(errno));
                        exit(1);
                    }
                    else {
                        for (int i = 0 ; i < read_to_shell; i++){
                            char a_char = buffer_to_shell[i];
                            if (a_char == '\r' ||a_char == '\n' ){
                                if (write(1, "\r\n", 2) < 0){
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                                if (write(to_shell[1], "\n", 1) < 0){
                                    fprintf(stderr,"Error in writing to shell, %s", strerror(errno));
                                    exit(1);
                                }
                            }
                            else if (a_char == 0x04){
                                if (write(1, "^D", 2) < 0){
                                    fprintf(stderr,"Error in writing eof to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                                if (debug_flag) fprintf(stderr, "^D received, loop breaking\r\n");
                                exit_flag = 1;
                                if (close(to_shell[1]) < 0) {
                                    fprintf(stderr, "Parent process: rror in closing to_shell[1], %s\n",strerror(errno));
                                    exit(1);
                                    
                                }
                                
                            }
                            else if (a_char == 0x03){
                                if (write(1, "^C", 2) < 0){
                                    fprintf(stderr,"Error in writing interrupt to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                                if (kill(ret_fork, SIGINT) < 0)
                                {
                                    fprintf(stderr, "Error in sending ginal SIGINT to shell: %s\n", strerror(errno));
                                    exit(1);
                                }
                                
                            }
                            else{
                                if (write(1, &a_char, 1) < 0){
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                                if (write(to_shell[1], &a_char, 1) < 0){
                                    fprintf(stderr,"Error in writing to shell, %s", strerror(errno));
                                    exit(1);
                                }
                            }
                        }
                    }
                }
                if (fds[1].revents & POLLIN) {
                    ssize_t read_to_termi = read(to_termi[0], buffer_to_termi, sizeof(buffer_to_termi));
                    if (read_to_termi < 0) fprintf(stderr,"fds[1]: error in reading before forward to stdout, %s", strerror(errno));
                    else {
                        for (int i = 0 ; i < read_to_termi; i++){
                            char b_char = buffer_to_termi[i];
                            if ( b_char == '\n' ){
                                if (write(1, "\r\n", 2) < 0){
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                            }
                            else if (b_char == 0x04){
                                if (write(1, "^D", 2) < 0){
                                    fprintf(stderr,"Error in writing eof to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                                if (debug_flag) fprintf(stderr, "^D received, looop breaking...\r\n");
                                exit_flag = 1;
                                
                            }
                            else{
                                if (write(1, &b_char, 1) < 0){
                                    fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                                    exit(1);
                                }
                            }
                        }
                    }
                }
                if (fds[0].revents & (POLLHUP | POLLERR))  {
                    if (debug_flag) fprintf(stderr,"Finish reading from stdin...\r\n");
                    exit_flag = 1;
                }
                if (fds[1].revents & (POLLHUP | POLLERR))  {
                    if (debug_flag) fprintf(stderr,"Finish reading from shell...\r\n");
                    exit_flag = 1;
                }
                if (exit_flag) {
                    if (debug_flag) fprintf(stderr, "Exit flag received, waiting for child and exit...\r\n");
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
                    if (write(0, "\r\n", 2) < 0){
                        fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                        exit(1);
                    }
                }
                else if (c_char == 0x04){
                    if (write(0, "^D", 2) < 0){
                        fprintf(stderr,"Error in writing eof to stdout, %s", strerror(errno));
                        exit(1);
                    }
                    if (debug_flag) fprintf(stderr, "^D received, looop breaking");
                    break_flag_no_shell = 1;
                }
                
                else{
                    if (write(0, &c_char, 1) < 0){
                        fprintf(stderr,"Error in writing to stdout, %s", strerror(errno));
                        exit(1);
                    }
                }
            }
            if (break_flag_no_shell) break;
        }
    }

    /**********finishing up*********/
    exit(0);
}

