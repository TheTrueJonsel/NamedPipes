#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <spawn.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include "parent.h"



const char *pipePath = "./my_pipe";
double firstNum, secondNum;
int fd;
double result;
sem_t *mutex;
sem_t *turn;
int turnValue;
int mutexValue;

int main()
{
    
    signal(SIGINT, handler);
    signal(SIGHUP, handler);
    signal(SIGTERM, handler);
    createPipe();
    fd = open(pipePath, O_RDWR);
    initMutex();
    //resetSemaphores();
    createChild();

    //sleep for 3 seconds to allow user to see status output
    sleep(3);
    clearConsole();

    while(1){
        getUserInput();
        
        writeToPipe();

        //unlocking mutex to allow childprocess to access the pipe
        sem_post(turn);
        sem_post(mutex);

        //locking the mutex to read the result from the pipe
        sem_getvalue(turn, &turnValue);
        while(turnValue != 0){
            sem_getvalue(turn, &turnValue);
        }
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
        printf("PARENT: successfully created pipe\n");
    } else {
        printf("PARENT: ERROR: could not create pipe\npipe-status: %d\n", pipeStatus);
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
        printf("PARENT: successfully created child process with pid: %d\n", pid);
    } else {
        printf("Failed to create child process. Status: %d\n", spawnStatus);
    }
}

void writeToPipe(){
    int writeStatus;

    //writing first number into the pipe
    writeStatus = write(fd, &firstNum, sizeof(double));
    if(writeStatus != -1){
        //printf("PARENT: Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("PARENT: ERROR - write operation failed with status: %d\n", writeStatus);
    }

    //writing second number into the pipe
    writeStatus = write(fd, &secondNum, sizeof(double));
    if(writeStatus != -1){
        //printf("PARENT: Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("PARENT: ERROR - write operation failed with status: %d\n", writeStatus);
    }
}

//ask user for input and store result in firstNum and secondNum then print the entered values to stdout
void getUserInput(){
        printf("Enter the two numbers that you want to add, seperated by a space:\n");
        scanf("%lf %lf", &firstNum, &secondNum);
        clearConsole();
        printf("You entered: %lf, %lf\n", firstNum, secondNum);
        printf("-------------------\n");
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
        //printf("PARENT: Successfully read %d Byte from the pipe\n", readStatus);
        printf("PARENT: The result of adding both numbers is: %lf\n", result);
    } else {
        printf("PARENT: ERROR - read operation failed with status: %d\n", readStatus);
    }

}

void initMutex(){
    //mutex
    mutex = sem_open("/mutex", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(mutex == SEM_FAILED){
        printf("PARENT: ERROR - could not open mutex\n");
    } else {
        printf("PARENT: successfully initialized mutex\n");
    }

    //turn variable to ensure that the Bounded Waiting criterion for mutual exclusion is satisfied
    turn = sem_open("/turn", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(turn == SEM_FAILED){
        printf("PARENT: ERROR - could not open turn-mutex\n");
    } else {
        printf("PARENT: Successfully initialized turn-mutex\n");
    }

}

//done for testing, can be ignored
#if 0
void resetSemaphores(){
    clearConsole();
    printf("PARENT: ------------------ RESETTING SEMAPHORES ------------------\n");

    //resetting mutex semaphore
    sem_getvalue(mutex, &mutexValue);
    printf("PARENT: former value of mutex-semaphore: %d\n", mutexValue);
    for(int i = 0; i < mutexValue; i++){
        sem_wait(mutex);
    }
    sem_getvalue(mutex, &mutexValue);
    printf("PARENT: current value of mutex-semaphore: %d\n", mutexValue);

    //resetting turn semaphore
    sem_getvalue(turn, &turnValue);
    printf("PARENT: former value of turn-semaphore: %d\n", turnValue);
    for(int i = 0; i < turnValue; i++){
        sem_wait(turn);
    }
    sem_getvalue(turn, &turnValue);
    printf("PARENT: current value of turn-semaphore: %d\n", turnValue);

    sleep(5);
    clearConsole();
}
#endif

void handler(){
    int removeStatus = remove("./my_pipe");
    if(removeStatus == 0){
        printf("\nPARENT: Successfully removed the pipe\nEXITING\n");
    } else {
        printf("\nPARENT: ERROR - could not remove file\n");
    }
    sleep(3);
    exit(0);
}