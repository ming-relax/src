#include <stdio.h>
#include <stdlib.h>

#define print_var(name)\
 printf("the addr of "#name" is: %p\n", name);

#define NAME_LEN 32
char global_name[NAME_LEN];


int f2(int i, long l)
{
	int t =0x22222222;
	char myname[NAME_LEN]; 
	return 0;
}

int f1()
{
	int rc, t=0x11111111;
	char myname[NAME_LEN]; 

	rc = f2(0x66666666, 0x88888888);
	return rc;
}




int main()
{
	char * name_on_heap = NULL;
	char name_on_stack[NAME_LEN];
	char *static_name = "my name";
	
	name_on_heap = malloc(NAME_LEN);

	print_var(global_name);
	print_var(name_on_stack);
	print_var(static_name);

	if(name_on_heap != NULL){
		print_var(name_on_heap);
		free(name_on_heap);
	}	

	print_var(fopen);
	print_var(f1);
	f1();

}
