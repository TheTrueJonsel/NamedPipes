#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "child.h"

const int fd;
const char *pipePath = "./my_pipe";

double firstNum, secondNum, result;

int main()
{
    fd = open(pipePath, O_RDWR);
    while(1){
        pthread_mutex_lock(&mutex);
        readFromPipe();
        result = firstNum + secondNum;
        writeToPipe();
        pthread_mutex_unlock(&mutex);
        
        
    }

}

//reads from the pipe and prints the output
void readFromPipe(){
    int readStatus;
    readStatus = read(fd, &firstNum, sizeof(double));
    if(readStatus != -1){
        printf("Successfully read %d Byte from the pipe\n", readStatus);
        printf("The number read from the pipe is: %lf\n", firstNum);
    } else {
        printf("ERROR - read operation failed with status: %d\n", readStatus);
    }

    //reads from the pipe and prints the output
    readStatus = read(fd, &secondNum, sizeof(double));
    if(readStatus != -1){
        printf("Successfully read %d Byte from the pipe\n", readStatus);
        printf("The number read from the pipe is: %lf\n", secondNum);
    } else {
        printf("ERROR - read operation failed with status: %d\n", readStatus);
    }
}

void writeToPipe(){
    int writeStatus = write(fd, &result, sizeof(double));
    if(writeStatus != -1){
        printf("Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("ERROR - write operation failed with status: %d\n", writeStatus);
    }
}