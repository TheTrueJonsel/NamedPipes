#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <spawn.h>
#include <unistd.h>
#include <semaphore.h>
#include "parent.h"


const char *pipePath = "./my_pipe";
double firstNum, secondNum;
int fd;
double result;
sem_t *mutex;

int main()
{
    createPipe();
    fd = open(pipePath, O_RDWR);
    initMutex();
    createChild();

    //sleep for 3 seconds to allow user to see status output
    sleep(3);
    clearConsole();

    while(1){
        getUserInput();
        
        writeToPipe();

        //unlocking mutex to allow childprocess to access the pipe
        sem_post(mutex);
        sleep(8);

        //locking the mutex to read the result from the pipe
        sem_wait(mutex);
        readFromPipe();
    }



    //deleting the file from disk after program is done running
    remove("./my_pipe");


    

    //override signal function to detroy the mutex when process gets terminated/killed
    //pthread_mutex_destroy(&mutex);
    
    return 0;
}

//creating pipe using mkfifo and checking if successfull
void createPipe(){
    int pipeStatus = mkfifo(pipePath, S_IRUSR | S_IWUSR);
    if(pipeStatus == 0){
        printf("successfully created pipe\n");
    } else {
        printf("ERROR: could not create pipe\npipe-status: %d\n", pipeStatus);
    }
}

//creating child process and checking if successful - then waiting 2 seconds so user can read status message
void createChild(){
    pid_t pid;

    //creating argument and environment vectors to start the child process with
    char *argv[] = {"./child", NULL};
    extern char **environ;

    //creating child process
    int spawnStatus = posix_spawn(&pid, "./child", NULL, NULL, argv, environ); 
    if(spawnStatus == 0){
        printf("successfully created child process with pid: %d\n", pid);
    } else {
        printf("Failed to create child process. Status: %d\n", spawnStatus);
    }
}

void writeToPipe(){
    int writeStatus;

    //writing first number into the pipe
    writeStatus = write(fd, &firstNum, sizeof(double));
    if(writeStatus != -1){
        printf("PARENT: Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("PARENT: ERROR - write operation failed with status: %d\n", writeStatus);
    }

    //writing second number into the pipe
    writeStatus = write(fd, &secondNum, sizeof(double));
    if(writeStatus != -1){
        printf("PARENT: Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("PARENT: ERROR - write operation failed with status: %d\n", writeStatus);
    }
}

//ask user for input and store result in firstNum and secondNum then print the entered values to stdout
void getUserInput(){
        printf("Enter the two numbers that you want to add, seperated by a space:\n");
        scanf("%lf %lf", &firstNum, &secondNum);
        printf("You entered: %lf, %lf\n", firstNum, secondNum);
}

void clearConsole(){
    for(int i = 0; i < 50; i++){
            printf("\n");
    }
}

//reads from the pipe and prints the output
void readFromPipe(){
    int readStatus = read(fd, &result, sizeof(double));
    if(readStatus != -1){
        printf("PARENT: Successfully read %d Byte from the pipe\n", readStatus);
        printf("PARENT: The number read from the pipe is: %lf\n", result);
    } else {
        printf("PARENT: ERROR - read operation failed with status: %d\n", readStatus);
    }

}

void initMutex(){
    mutex = sem_open("/mutex", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(mutex == SEM_FAILED){
        printf("PARENT: ERROR - could not open mutex\n");
    } else {
        printf("PARENT: successfully initialized mutex\n");
    }
}