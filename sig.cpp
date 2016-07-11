#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
       #include <sys/types.h>
       #include <unistd.h>
#include <string.h>

pthread_t subtid[10];
int checkpending();

#define SUB_SIG SIGTERM 
/*signal handler for sub threads*/
void sub_handler(int signo)
{
	printf("thread: %d, got signal %d \n", pthread_self(), signo);
	pthread_exit(NULL);
}

int block_signal(int signo)
{
        sigset_t    newset, oldset;
	int rc = 0;

	sigemptyset(&newset);
	sigaddset(&newset, signo);
	rc = pthread_sigmask(SIG_BLOCK,&newset, &oldset);
	return rc;
}

int unblock_signal(int signo)
{
        sigset_t    newset, oldset;
	int rc = 0;

	sigemptyset(&newset);
	sigaddset(&newset, signo);
	rc = pthread_sigmask(SIG_UNBLOCK,&newset, &oldset);
	return rc;
}

void * sub_thread(void *data)
{
        struct sigaction    act, oact;
        int rc = 0;
        act.sa_handler = sub_handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        if (sigaction(SUB_SIG, &act, &oact) < 0)
        {
                perror("error setting handler\n");
		goto end;
        }

	checkpending();

	rc = block_signal(SUB_SIG);
	if(rc < 0){
		perror("error setting mask\n");
		goto end;
	}

	checkpending();
	sleep(20);
	printf("Hi, tid: %d, sleep ok\n", pthread_self());

	checkpending();

	rc = unblock_signal(SUB_SIG);
	if(rc < 0){
		perror("error setting mask\n");
		goto end;
	}
end:
	;
}

/*create many subthreads, thread IDs will be saved in subtid[]*/
int create_threads()
{
	int i;
	memset((void*)subtid, -1, 10*sizeof(pthread_t));
	for(i = 0; i < 10; i++){
		if(pthread_create(&subtid[i], NULL, sub_thread, (void *)NULL) != 0)
		{
			perror("pthread_create failed\n");
			return -1;
		} else{
			printf("created new thread:%d\n", subtid[i]);
			pthread_detach(subtid[i]);
		}
	}
}

/*signal handler for SIGUSR1, used by main thread*/
void usr1_func(int signo)
{
	int i;
	printf("signal %d delivered to me\n", signo);
	for(i = 0; i < 10; i++){
		if(subtid[i] != -1)
			pthread_kill(subtid[i], SUB_SIG);
	}
	printf("kill subthreads ok\n", signo);
}

/*check pending signals and print them*/
int checkpending()
{
	sigset_t pendset[1];
	sigemptyset(pendset);
	int rc;

	if(sigpending(pendset)){
		perror("sigpending error\n");
		return -1;
	}
	rc = sigismember(pendset, SUB_SIG);
	if(rc == 1){
		printf("sig: %d is pending\n", SUB_SIG);
	}else if(rc == 0){
		printf("sig: %d NOT pending\n", SUB_SIG);
	}else{
		perror("sigismember\n");
	}
}

int main()
{
	struct sigaction    act, oact;
	sigset_t    newset, oldset;
	int rc = 0;
	int pid;
	int i;

	act.sa_handler = usr1_func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(SIGUSR1, &act, &oact) < 0)
	{
		perror("error setting handler\n");
        	return(-1);
	}

	while(1){
		block_signal(SIGUSR1);
		create_threads();
		sleep(2);
		unblock_signal(SIGUSR1);
		printf("Hi, I will sleep, pid: %d, please send me signal: %d\n", getpid(), SIGUSR1);
		pause();
	
	}
	
	return 0;	
}

