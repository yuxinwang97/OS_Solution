#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <ctype.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>


#define B 4275
#define R0 100000.0

//TODO check for system call

int period = 1;
int opt_fault = 0;
FILE* log_name;
char unit = 'F';
char* buffer;
char* temp_buffer;
char* which_host;
int myid;
int port_num;
int socketfd;
int stop = 0;
int end = 0;
int log_flag = 0;
int socketfd;
SSL_CTX* ctx;
SSL* sslClient;
mraa_aio_context temperature; 
//mraa_gpio_context button; 
struct timespec tm;
struct tm* local_time;
int check_time = 0;
int break_flag = 0;
struct hostent *server;
struct sockaddr_in addr;

static struct option options[] = {
    {"period", 1, NULL, 'p'},
    {"log"   , 1, NULL, 'l'},
	{"scale" , 1, NULL, 's'},
    {"host" , 1, NULL, 'h'},
    {"id" , 1, NULL, 'i'},
    {0, 0, 0, 0}
};

void print_err(char* errstr) {
        fprintf(stderr,"Error in: %s", errstr);
        exit(2);
}

int timer(char* input_str){
    clock_gettime(CLOCK_REALTIME, &tm);
	if (tm.tv_sec >= check_time && !stop){
		local_time = localtime(&(tm.tv_sec));
		//char str[1024];
		snprintf(input_str, 1024, "%02d:%02d:%02d",local_time->tm_hour,local_time->tm_min,local_time->tm_sec);//TODO error checking!
		//printf("%s",str);
		/*
		if (write(1, str, strlen(str)) < 0)
				print_err("fail to write id");
		if (log_flag) fputs(str,log_name); //TODO error handling
		*/
		check_time = tm.tv_sec + period;
		return 1;
	}
	else return 0;

}

void stopbutton(){
	clock_gettime(CLOCK_REALTIME, &tm);
	local_time = localtime(&(tm.tv_sec));
	char str[1024];
	snprintf(str, 1024, "%02d:%02d:%02d SHUTDOWN\n",local_time->tm_hour,local_time->tm_min,local_time->tm_sec);//TODO error checking!
	//printf("%s",str);
	if (SSL_write(sslClient,str,strlen(str)) <= 0) {print_err("fail to write shutdown");}
	if (log_flag) fputs(str,log_name); //TODO error handling
	exit(0);
}

void process_read(){
	if (buffer == NULL) return;
	if (strncmp(buffer, "SCALE=F", strlen("SCALE=F")) == 0) {
		unit = 'F';
		if (log_flag) {fputs(buffer,log_name); fputs("\n",log_name);}
	}
	else if (strncmp(buffer, "SCALE=C", strlen("SCALE=C")) == 0) {
		unit = 'C';
		if (log_flag) {fputs(buffer,log_name); fputs("\n",log_name);}
	}
	else if (strncmp(buffer, "STOP" , strlen("STOP")) == 0) {
		stop = 1;
		if (log_flag) {fputs(buffer,log_name); fputs("\n",log_name);}
	}
	else if (strncmp(buffer, "START" , strlen("START")) == 0) {
		stop = 0;
		if (log_flag) {fputs(buffer,log_name); fputs("\n",log_name);}
	}
	else if (strncmp(buffer, "OFF" , strlen("OFF")) == 0) {
		if (log_flag)  {fputs(buffer,log_name); fputs("\n",log_name);}
		stopbutton();
	}
	else if (strncmp(buffer, "PERIOD=", strlen("PERIOD=")) == 0) {
		period = atoi(buffer+7);
		if (log_flag)  {fputs(buffer,log_name); fputs("\n",log_name);}
	}
	else if (strncmp(buffer, "LOG", strlen("LOG")) == 0) {
		if (log_flag)  {fputs(buffer,log_name); fputs("\n",log_name);}
	}
}

void before_exit(){
    if (buffer != NULL) free(buffer);
	if (temp_buffer != NULL) free(temp_buffer);
	if (log_name != NULL && log_flag == 1) fclose(log_name);
    if (which_host != NULL) free(which_host);
	mraa_aio_close(temperature);
	if (sslClient != NULL) {
		SSL_shutdown(sslClient);
		SSL_free(sslClient);
		//free(sslClient);
	}
	if (ctx != NULL) free(ctx);
    //mraa_gpio_close(button);
}


float convert_temper_reading(int reading)
{
	float R = 1023.0/((float) reading) - 1.0;
	R = R0*R;
	//C is the temperature in Celcious
	float C = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
	if (unit == 'C') {return C;}
	else {
		//F is the temperature in Fahrenheit
		float F = (C * 9)/5 + 32;
		return F;
	}
}

void setup_socket() {
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { 
        print_err("fail to create socket\n");
    }
    if ((server = gethostbyname(which_host)) == NULL)  { 
        print_err("fail to get host by name\n");
    }
    //memset((char*)&addr, 0, sizeof(addr));
	bzero((char *)&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    bcopy((char *)(server -> h_addr), (char *)&addr.sin_addr.s_addr, server->h_length);
	addr.sin_port = htons(port_num);
    if(connect(socketfd, (struct sockaddr*)&addr, sizeof(addr))< 0) {
        print_err("fail to connect to socket\n");
    }
    if(SSL_library_init() < 0){
        print_err("fail to initl ssl\n");
    }
    SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
    SSL_CTX* ctx =  SSL_CTX_new(TLSv1_client_method());
    if (ctx == NULL){
        print_err("initialize new SSL_CTX\n");
    }
    sslClient = SSL_new(ctx);
	if (SSL_set_fd(sslClient, socketfd) <= 0) {
        print_err("error using SSL_set_fd\n");
    }
    if (SSL_connect(sslClient) != 1){
        print_err("error using SSL_connect\n");
    }

    char id_buffer[50];
	sprintf(id_buffer, "ID=%d\n", myid);
	//TODO DELETE
	//printf("%s",id_buffer);
	if (SSL_write(sslClient,id_buffer,strlen(id_buffer)) <= 0) {print_err("fail to print id");}
	if (log_flag) fputs(id_buffer,log_name);
}


int main(int argc, char * argv[]) {

	int opt;

	while ((opt = getopt_long(argc, argv, "", options, &opt_fault)) != -1){
        switch(opt){
			case 'p': 
				period = atoi(optarg);
				break;

			case 'l':
				log_name = fopen(optarg, "w");
            	if(log_name == NULL) {
					print_err("openfile");
				}
				log_flag = 1;
				break;

			case 's':
				if (optarg[0] == 'C') unit = 'C';
				break;

            case 'i':
                if (strlen(optarg) != 9) print_err("id number needs exactly 9 digits");
                myid = atoi(optarg);
                break;

            case 'h':
                which_host = malloc((strlen(optarg)+1)*sizeof(char));
                memcpy(which_host,optarg,(strlen(optarg)+1)*sizeof(char));
                break;

			default:
				fprintf(stderr,"Error in finding unrecognized argument %c\n",(char)optopt);
                fprintf(stderr,"Correct Usage: ./lab4b --period=[seconds] --scale=[C/F] --log=[filename]\n");
                exit(1);
				break;
		}
	}

    if (atexit(before_exit) < 0) {
        fprintf(stderr, "Error: cannot set exit function\n");
        exit(2);
    }

    if (optind >= argc) {
		fprintf(stderr,"no port number!\n");
        exit(2);
	}
    else port_num = atoi(argv[optind]);

    setup_socket();

	temperature = mraa_aio_init(1);
    if(temperature == NULL) {print_err("init temperature sensor");}
	/*
    button = mraa_gpio_init(60);
	if (button == NULL) {print_err("init button");
	}
    mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &stopbutton, NULL);
	*/

	struct pollfd fds[1];
	fds[0].fd = socketfd;
	fds[0].events = POLLIN;
	//fcntl(0, F_SETFL, O_NONBLOCK);

	temp_buffer = (char*) malloc(sizeof(char)*256);
	buffer = (char*) malloc(sizeof(char)*256);
	if (buffer ==NULL) print_err("initializing buffer");
	int is_time;

	while (1) {
		char time_ret[1024];
		is_time = timer(time_ret);
		if ( is_time && !stop){
			int tempera = mraa_aio_read(temperature);
			float desired_t = convert_temper_reading(tempera);
			char str[1024];
			snprintf(str, 1024, "%s %0.1f\n",time_ret, desired_t);//TODO error checking!
			//TODO DELETE
			//printf("%s",str);
            if (SSL_write(sslClient,str,strlen(str)) < 0){
                print_err("fail to write to server\n");
            }
			if (log_flag) fputs(str,log_name);
		}

		int rpoll = poll(fds, 1, 1);
		if ( rpoll == -1) {
            print_err("poll");
        }
		else if (rpoll > 0) {
			if ((fds[0].revents & POLLIN)) {  
				//ssize_t read_buffer = read(0,temp_buffer, sizeof(temp_buffer));
				ssize_t read_buffer = SSL_read(sslClient,temp_buffer, sizeof(temp_buffer));
				//TODO error handling
				//if (read_buffer < 0) fprintf(stderr,"error in reading before forward to stdout");
				if (read_buffer > 0) {
					int i;
					for (i = 0; i < read_buffer; i++){
						if (temp_buffer[i] == '\n'){
							buffer[end] = '\0';
							end = 0;
							process_read();
						}
						else{
							buffer[end] = temp_buffer[i];
							end++;
						}
					}

				}
			} 	
		}

		if (break_flag)  break;

	}

	exit(0);
}