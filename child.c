#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include "child.h"

int fd;
const char *pipePath = "./my_pipe";
sem_t *mutex;
sem_t * turn;

double firstNum, secondNum, result;
int turnValue;

int main()
{
    fd = open(pipePath, O_RDWR);
    initMutex();
    while(1){
        sem_getvalue(turn, &turnValue);
        while(turnValue != 1){
            sem_getvalue(turn, &turnValue);
        }
        sem_wait(mutex);
        readFromPipe();
        result = firstNum + secondNum;
        writeToPipe();
        sem_wait(turn);
        sem_post(mutex);
    }
}

//reads from the pipe and prints the output
void readFromPipe(){
    int readStatus;
    readStatus = read(fd, &firstNum, sizeof(double));
    if(readStatus != -1){
        //printf("CHILD: Successfully read %d Byte from the pipe\n", readStatus);
        //printf("CHILD: The number read from the pipe is: %lf\n", firstNum);
    } else {
        printf("CHILD: ERROR - read operation failed with status: %d\n", readStatus);
    }

    //reads from the pipe and prints the output
    readStatus = read(fd, &secondNum, sizeof(double));
    if(readStatus != -1){
        //printf("CHILD: Successfully read %d Byte from the pipe\n", readStatus);
        //printf("CHILD: The number read from the pipe is: %lf\n", secondNum);
    } else {
        printf("CHILD: ERROR - read operation failed with status: %d\n", readStatus);
    }
}

void writeToPipe(){
    int writeStatus = write(fd, &result, sizeof(double));
    if(writeStatus != -1){
        //printf("CHILD: Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("CHILD: ERROR - write operation failed with status: %d\n", writeStatus);
    }
}

void initMutex(){
    //mutex
    mutex = sem_open("/mutex", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(mutex == SEM_FAILED){
        printf("CHILD: ERROR - could not open mutex\n");
    } else {
        printf("CHILD: Successfully opened mutex\n");
    }

    //turn variable to ensure that the Bounded Waiting criterion for mutual exclusion is satisfied
    turn = sem_open("/turn", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(turn == SEM_FAILED){
        printf("CHILD: ERROR - could not open turn-mutex\n");
    } else {
        printf("CHILD: Successfully initialized turn-mutex\n");
    }
}
