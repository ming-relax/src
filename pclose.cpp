#include <iostream>
#include <string>
using std::string;
using std::cout;

#include <stdio.h>
#include <pthread.h>

int callPopen(const char* cmd, string& retstr)
{
    FILE *file;
    bool empty = true;
    int status;
    char line[1024];
    string store = retstr;

    if (cmd == NULL) {
        return -1; 
    }   

    if((file = popen(cmd, "r")) == NULL) {
        return -1; 
    }   

    while(fgets(line, 1024, file) != NULL) {
        empty = false;

        retstr = retstr + line;
    }   

    if ((status = pclose(file)) < 0) {
        retstr = store;
        return -1; 
    } else {    
        if (!WIFEXITED(status)) {
            retstr = store;
            return -1; 
        } else {
            if (empty == true) {
                if (WEXITSTATUS(status)) {
                    retstr = string(cmd) + ": command not found or failed.";
                }   
            }   
        }   
    }   

    return WEXITSTATUS(status);
}


int run()
{
	string  rslt;
	while(1){
		callPopen("ls -l", rslt);
		cout <<"tid:"<<pthread_self() <<":\n"<< rslt;
	}
}
void * thread1(void *)
{
	run();
}

int main()
{
	pthread_t tid;
	pthread_create(&tid, NULL, thread1, NULL);
	pthread_create(&tid, NULL, thread1, NULL);
	run();
}
