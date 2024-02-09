#include <stdio.h>
#include <pthread.h>

//creating mutex
pthread_mutex_t mutex;

int main()
{
    //named pipe erstellen
    //zwei zahlen als als userinput erhalten
    //beide zahlen in die named pipe schreiben
    //kindprozess erstellen über posix_spawn
    //kindprozess beide zahlen aus der named pipe auslesen lassen
    //kindprozess addiert beide zahlen
    //kindprozess schreibt ergebnis der addition in die pipe zurück
    //Elternprozess liest Ergebnis aus der pipe und gibt es über stdout aus
    //Elternprozess wartet dann erneut aus userinput

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