#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <spawn.h>
#include <unistd.h>
#include "parent.h"


pthread_mutex_t mutex;
const char *pipePath = "./my_pipe";
double firstNum, secondNum;
int fd;
double result;

int main()
{
    createPipe();

    //opening the pipe with read and write permissions
    fd = open(pipePath, O_RDWR);

    //initializing mutex
    pthread_mutex_init(&mutex, NULL);

    createChild();

    //sleep for 3 seconds to allow user to see status output
    sleep(3);

    //clear up the standard output after all of the status outputs
    clearConsole();

    pthread_mutex_lock(&mutex);
    while(1){
        getUserInput();
        
        writeToPipe();

        //unlocking mutex to allow childprocess to access the pipe
        pthread_mutex_unlock(&mutex);

        //locking the mutex to read the result from the pipe
        pthread_mutex_lock(&mutex);
        readFromPipe();
    }



    //deleting the file from disk after program is done running
    remove("./my_pipe");


    

    //override signal function to detroy the mutex when process gets terminated/killed
    pthread_mutex_destroy(&mutex);
    
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
        printf("Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("ERROR - write operation failed with status: %d\n", writeStatus);
    }

    //writing second number into the pipe
    writeStatus = write(fd, &secondNum, sizeof(double));
    if(writeStatus != -1){
        printf("Successfully wrote %d Byte into the pipe\n", writeStatus);
    } else {
        printf("ERROR - write operation failed with status: %d\n", writeStatus);
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
        printf("Successfully read %d Byte from the pipe\n", readStatus);
        printf("The number read from the pipe is: %lf\n", result);
    } else {
        printf("ERROR - read operation failed with status: %d\n", readStatus);
    }

}