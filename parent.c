#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

//declaring mutex
pthread_mutex_t mutex;
const char *path = "./my_pipe";

int main()
{
    
    //creating pipe
    int status = mkfifo(path, S_IRUSR | S_IWUSR);
    if(status == 0){
        printf("successfully created pipe\npipe-status: %d\n", status);
    } else {
        printf("ERROR: could not create pipe\npipe-status: %d\n", status);
    }

    //opening the pipe with read and write permissions
    int fd = open(path, O_RDWR);


    //deleting the file from disk after program is done running
    remove("./my_pipe");

    //initializing mutex
    pthread_mutex_init(&mutex, NULL);

    //locking the mutex
    pthread_mutex_lock(&mutex);
    // <--critical region-->

    //unlocking the mutex
    pthread_mutex_unlock(&mutex);

    //override signal function to detroy the mutex when process gets terminated/killed
    pthread_mutex_destroy(&mutex);
    
    return 0;
}