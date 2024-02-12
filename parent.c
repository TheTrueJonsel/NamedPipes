#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <spawn.h>

//declaring mutex
pthread_mutex_t mutex;
const char *pipePath = "./my_pipe";

int main()
{
    
    //creating pipe
    int pipeStatus = mkfifo(pipePath, S_IRUSR | S_IWUSR);
    if(pipeStatus == 0){
        printf("successfully created pipe\npipe-status: %d\n", pipeStatus);
    } else {
        printf("ERROR: could not create pipe\npipe-status: %d\n", pipeStatus);
    }

    //opening the pipe with read and write permissions
    int fd = open(pipePath, O_RDWR);

    //creating child process
    pid_t pid;

    //creating argument and environment vectors to start the child process with
    const char *argv[] = {"./child", NULL};
    const char *envp[] = {NULL};

    //creating child process
    int spawnStatus = posix_spawn(&pid, "./child", NULL, NULL, argv, envp); 
    if(spawnStatus == 0){
        printf("successfully created child process with pid: %d\n", pid);
    } else {
        printf("Failed to create child process. Status: %d\n", spawnStatus);
    }

    


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