/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby,
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
pthread_mutex_t lock;
pthread_t ping_t, pong_t;
void* ping(void* args);
void* pong(void* args);
int *contador = 0;
int *flag= 0;
int pipe1[2];
void cantidad(int n) {
	int cantidad = *contador;
	printf("cantidad ping y pongs %d\n", cantidad);

	write(pipe1[1], &cantidad, sizeof(int));
	*flag=1;

}
int main()
{	int n = 1;
	pipe(pipe1);
	signal(SIGINT, cantidad);
	contador  = mmap(NULL, sizeof* contador,
	                 PROT_READ | PROT_WRITE,
	                 MAP_SHARED | MAP_ANONYMOUS,
	                 -1, 0);
	flag  = mmap(NULL, sizeof* flag,
	             PROT_READ | PROT_WRITE,
	             MAP_SHARED | MAP_ANONYMOUS,
	             -1, 0);
	*contador = 1;
	*flag = 0;
	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig, SIGUSR1);
	sigaddset(&sig, SIGUSR2);
	sigprocmask(SIG_BLOCK, &sig, NULL);
	pthread_create(&ping_t, NULL, &ping, NULL);

	pthread_create(&pong_t, NULL, &pong, NULL);
	usleep(1);
	pthread_kill(ping_t, SIGUSR1);
	read(pipe1[0], &n,sizeof(int));
	FILE *archivo;
	archivo = fopen("test.txt", "w");
	fprintf(archivo, "total de pings y pongs: %d", n);
	fclose(archivo);
	pthread_join(ping_t, NULL);
	pthread_join(pong_t, NULL);

	return 0;
}

void* ping(void* args) {
	int n = 0;
	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig, SIGUSR1);

	while(1) {
		if(*flag==1) {
			sigwait(&sig,  &n);
			pthread_mutex_lock(&lock);
			printf("ping \n");

			sleep(1);
			*contador= *contador + 1;
			pthread_kill(pong_t, SIGUSR2);
			pthread_mutex_unlock(&lock);
		}
		else {
			return NULL;
		}


	}
}
void* pong(void* args) {
	int n = 0;
	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig, SIGUSR2);

	while(1) {
		if(*flag==0) {
			sigwait(&sig,  &n);

			pthread_mutex_lock(&lock);
			printf("pong \n");
			sleep(1);
			*contador= *contador + 1;
			pthread_mutex_unlock(&lock);
			pthread_kill(ping_t, SIGUSR1);
		} else {
			return NULL;
		}



	}
}
