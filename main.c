#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "queue.h"
#define NCLERKTHREDS 5


struct customer{ //use this struct to record the customer information read from customers.txt
    int id;
    int service_time;
    int arrival_time;
    int class;
    double start;
    double end;

};

struct clerk{
    int struct_clerk_id;
};

pthread_mutex_t  queue1_mutex;
pthread_mutex_t  queue2_mutex;

pthread_cond_t queue1_cv;
pthread_cond_t queue2_cv;

pthread_mutex_t time_mutex;

pthread_mutex_t clerk0_mutex;
pthread_mutex_t clerk1_mutex;
pthread_mutex_t clerk2_mutex;
pthread_mutex_t clerk3_mutex;
pthread_mutex_t clerk4_mutex;

pthread_cond_t clerk0_cv;
pthread_cond_t clerk1_cv;
pthread_cond_t clerk2_cv;
pthread_cond_t clerk3_cv;
pthread_cond_t clerk4_cv;


void read_file(FILE *file, int size, struct customer p_list[] );
void* passenger_handler(void* passenger);
void* clerk_handler();


//global variables
double total_time;

int q1_len;
int q2_len;
Queue* q1;// business class
Queue* q2;// economy class

void* clerk_handler(){

}





void* passenger_handler(void * p_info){
    struct customer * passenger = (struct customer *) p_info;
    usleep(passenger->arrival_time*100000);

    fprintf(stdout, "A customer arrives: customer ID %2d. \n", passenger->id);
    if (passenger->class==1){
        pthread_mutex_lock(&queue1_mutex);
        enqueue(q1,passenger->id);
        q1_len++;
    } else{
        pthread_mutex_lock(&queue2_mutex);
        enqueue(q2,passenger->id);
        q2_len++;
    }


}


void read_file(FILE *file, int size, struct customer p_list[]) {




    int id, class, a_time, s_time;
    // Read and discard a line
    for (int i = 0; i < size; i++) {


        //copy the passenger info into a struct
        fscanf(file, "%d:%d,%d,%d",&id,&class,&a_time,&s_time);
        p_list[i].id = id;
        p_list[i].class = class;
        p_list[i].arrival_time = a_time;
        p_list[i].service_time = s_time;
        //printf("%d,%d, %d, %d\n",p_list[i].id,p_list[i].class, p_list[i].arrival_time, p_list[i].service_time);

    }


    fclose(file);
}

int main(int argc, char *argv[]) {
    // open file
    char* filename = argv[1];
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("unable to read file \n");

    }

    char buff[200];
    fgets(buff, 200, file);
    int size = atoi(buff); //get num of passengers and remove first line

    struct customer passenger_list[size];
    read_file(file, size, passenger_list); // add all passengers to a list

    for (int i = 0; i < size; ++i) {
        //printf("%d/n",i);
        printf("%d,%d,%d,%d\n", passenger_list[i].id, passenger_list[i].class, passenger_list[i].arrival_time, passenger_list[i].service_time);

    }

    //initialize clerks mutexes
    if (pthread_mutex_init(&clerk0_mutex, NULL)){
        printf("clerk_handler mutex initialization error\n");
        exit(0);
    }
    if (pthread_mutex_init(&clerk1_mutex, NULL)){
        printf("clerk_handler mutex initialization error\n");
        exit(0);
    }
    if (pthread_mutex_init(&clerk2_mutex, NULL)){
        printf("clerk_handler mutex initialization error\n");
        exit(0);
    }
    if (pthread_mutex_init(&clerk3_mutex, NULL)){
        printf("clerk_handler mutex initialization error\n");
        exit(0);
    }
    if (pthread_mutex_init(&clerk4_mutex, NULL)){
        printf("clerk_handler mutex initialization error\n");
        exit(0);
    }

    // initialize queue mutexes
    if (pthread_mutex_init(&queue1_mutex, NULL)){
        printf("queue mutex initialization error\n");
        exit(0);
    }
    if (pthread_mutex_init(&queue2_mutex, NULL)){
        printf("queue mutex initialization error\n");
        exit(0);
    }

    // time mutex
    if(pthread_mutex_init(&time_mutex, NULL)){
        printf("Error initializing time mutex.\n");
        exit(0);
    }
    // clerk_handler cv
    if(pthread_cond_init(&clerk0_cv, NULL)){
        printf("Error at clerk_handler 0 cv\n");
        exit(0);
    }

    if(pthread_cond_init(&clerk1_cv, NULL)){
        printf("Error at clerk_handler 1 cv\n");
        exit(0);
    }

    if(pthread_cond_init(&clerk2_cv, NULL)){
        printf("Error at clerk_handler 2 cv\n");
        exit(0);
    }

    if(pthread_cond_init(&clerk3_cv, NULL)){
        printf("Error at clerk_handler 3 cv\n");
        exit(0);
    }

    if(pthread_cond_init(&clerk4_cv, NULL)){
        printf("Error at clerk_handler 4 cv\n");
        exit(0);
    }
    // create clerks
    struct clerk clerk_list[NCLERKTHREDS];
    pthread_t clerk_id[NCLERKTHREDS-1];

    for (int i = 0; i < NCLERKTHREDS; ++i) {
        if(pthread_create(&clerk_id[i], NULL, clerk_handler, (void *) &clerk_list[i]) != 0){
            printf("can't create clerk_handler thread %d\n", i);
        }

    }
    // create passengers
    pthread_t passenger_id[size];
    for (int i = 0; i < size; ++i) {
        if(pthread_create(&passenger_id[i], NULL,passenger_handler,(void *) &passenger_list[i])!=0){
            printf("can't create passenger thread %d\n", i);
        }

    }

    // calculate waiting time

    return 0;
    //add passenger list to two queues
    // setup 5 clerks to process these
    // setup mutex locks
    // setup convar

}






