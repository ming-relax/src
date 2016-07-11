#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MSG_SZ 100

typedef struct user_data_s{
	pthread_mutex_t mutex;
	char msg[MSG_SZ];
}user_data_t;

typedef struct comm_s{
	pthread_mutex_t mutex;
	int ref_cnt;
	user_data_t* data;
}comm_t;

void comm_get(comm_t * p)
{
	pthread_mutex_lock(&p->mutex);
	if(p->ref_cnt == 0){
		p->data=malloc(sizeof(user_data_t));
		pthread_mutex_init(&p->data->mutex, NULL);
		snprintf(p->data->msg, MSG_SZ, "hello world");
	}
	p->ref_cnt ++;
	pthread_mutex_unlock(&p->mutex);
}


void comm_put(comm_t * p)
{
	pthread_mutex_lock(&p->mutex);
	p->ref_cnt --;
	if(p->ref_cnt == 0){
		
		pthread_mutex_destroy(&p->data->mutex);
		free(p->data);
		p->data=NULL;
	}
	pthread_mutex_unlock(&p->mutex);

}

void * thread_func1(void * param)
{
	int command = 1;
	comm_t *pcomm=(comm_t*)param;
	
	sleep(20);
	printf("you can modify command here\n");
	switch(command){
		case 0:
			printf("zero, do nothing\n");	
			break;
		case 1:
			comm_get(pcomm);
			printf("doing sth with lock ref cnd hold\n");	
			pthread_mutex_lock(&pcomm->data->mutex);
			printf("data:%s\n", pcomm->data->msg);	
			strcat(pcomm->data->msg, "from BWRAID");	
			pthread_mutex_unlock(&pcomm->data->mutex);
			comm_put(pcomm);
			break;
		default:	
			printf("I don't understand this\n");	
			break;
	}
	
}
void * thread_func2(void * param)
{
	int command = 1;
	int *p=NULL;
	comm_t *pcomm=(comm_t*)param;
	
	sleep(30);
	printf("you can modify command here\n");
	switch(command){
		case 0:
			printf("zero, do nothing\n");	
			break;
		case 1:
			comm_get(pcomm);
			printf("doing sth with lock ref cnd hold\n");	
			pthread_mutex_lock(&pcomm->data->mutex);
			printf("data:%s", pcomm->data->msg);	
			strcat(pcomm->data->msg, "BWRAID");	
			pthread_mutex_unlock(&pcomm->data->mutex);
			comm_put(pcomm);
			break;
		default:	
			printf("I don't understand this\n");	
			break;
	}
	
}

comm_t g_comm;


int create_thread(void*(*func)(void *), void*param, pthread_t *tid)
{
	int rc;
	rc = pthread_create(tid, NULL, func, param);
	if(rc != 0){
		perror("pthread create failed\n");
	}else{
		printf("created new thread: %d\n", *tid);
//		pthread_detach(tid);
	}
	return rc;
}

int main()
{
	pthread_t tid1, tid2;
	int rc, i;

	pthread_mutex_init(&g_comm.mutex, NULL);
	g_comm.ref_cnt=0;
	g_comm.data=NULL;

	comm_get(&g_comm);
	create_thread(thread_func1, &g_comm, &tid1);
	create_thread(thread_func2, &g_comm, &tid2);

	comm_put(&g_comm);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_mutex_destroy(&g_comm.mutex);

}
