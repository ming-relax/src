#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM 3
#define LEN 10
int gather_names(char**p)
{
	int i =0;
	while(!feof(stdin)){
		scanf("%s", p[i++]);
	}
}

void handle_names(char **p)
{
	int i =0;
	for(i=0; i < NUM; i++){
		if(p[i][0] == '_')
			printf("%s\n", p[i]);
	}
}

int main()
{
	char name[NUM][LEN];
	char *p[NUM];
	int i, rc = 0;

	memset(p, 0, sizeof(char*)*NUM);
#if defined(_DALLOC_)
	for(i = 0; i < NUM; i ++){
		p[i] = malloc(LEN);
		if(p[i] == NULL){
			rc = -1;
			goto out;
		}
	}
#else
	for(i = 0; i < NUM; i ++){
		p[i] = name[i];
	}
#endif
	gather_names(p);
	handle_names(p);
out:
#if defined(_DALLOC_)
	for(i = 0; i < NUM; i ++){
		if(p[i]){
			free(p[i]);
			p[i] = NULL;
		}
	}
#endif
	return rc;
}
