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
#define NCLERKTHREADS 5


struct customer{
    int id;
    int service_time;
    int arrival_time;
    int class;
    double start;
    double end;

};

int size;
int count;

pthread_mutex_t  queue1_mutex;
pthread_mutex_t  queue2_mutex;
pthread_mutex_t count_mutex;
pthread_cond_t queue1_cv;
pthread_cond_t queue2_cv;

pthread_mutex_t time_mutex;


pthread_mutex_t clerk_sig_mutex;
pthread_mutex_t q1_status_mutex;
pthread_mutex_t q2_status_mutex;



pthread_cond_t clerk_sig_cv;



void read_file(FILE *file, int size, struct customer p_list[] );
void* passenger_handler(void* p_info);

void* clerk_handler(void* clerk_id);


//global variables
double total_time;
int clerk_signal;

int q1_len;
int q2_len;
Queue* q1;// business class
Queue* q2;// economy class
struct customer passenger_list[100];

void* clerk_handler(void* clerk_id) {
    usleep(100);
    int cid = *(int *)clerk_id;
    printf("clerk id %d\n", cid);

    struct customer passenger;
    int qid;

    while (1) {
        if(count == size){
            printf("thread %d done\n", cid);
            break;
        }

        qid = 0;

        pthread_mutex_lock(&queue1_mutex);
        if (q1_len != 0) { //check if business queue is not empty
            printf("q1 selected%d\n", cid);
            qid =1;


        }
        if(qid != 1)// if not in business
        {
            pthread_mutex_lock(&queue2_mutex);
            if (q2_len != 0) { // Businness empty pick econ
                printf("q2 selected%d\n", cid);
                qid = 2;

            }
        }

        if(qid == 0){  //sleep if no customers and check again
            pthread_mutex_unlock(&queue1_mutex);
            pthread_mutex_unlock(&queue2_mutex);
            printf("clerk %d sleeping\n", cid);
            usleep(11);

            if ( count == size ){
                printf("loop done");
                break;
            }
            continue;

         }
        //will always pick business queue over economy queue



        // we have 2 queues with all the customers being added to them
        //wait for q1 to be clear

        if(qid ==1) { // serving business queue
            printf("q1 begin serving\n ");

            while (pthread_mutex_trylock(&q1_status_mutex) != 0) {

                usleep(11);

            }
            pthread_mutex_lock(&queue1_mutex);
            passenger = passenger_list[dequeue(q1)]; //get passenger info
            q1_len--;
            usleep(passenger.service_time*100000);
            pthread_mutex_lock(&count_mutex);
            count++;
            printf("count :%d\n", count);
            pthread_mutex_unlock(&count_mutex);
            printf(" q2 passenger serverd %d\n", passenger.id     );
            pthread_mutex_unlock(&queue1_mutex);
            pthread_mutex_unlock(&q1_status_mutex);


        }
        //wait for q2 to be clear
        if(qid ==2){

            printf("q2 begin serving\n ");
            while (pthread_mutex_trylock(&q2_status_mutex) != 0) {

                usleep(11);

            }
            pthread_mutex_lock(&queue2_mutex);
            passenger = passenger_list[dequeue(q2)]; //get passenger info
            q2_len--;
            usleep(passenger.service_time*100000);
            pthread_mutex_lock(&count_mutex);
            count++;
            printf("count :%d\n", count);
            pthread_mutex_unlock(&count_mutex);
            printf(" q2 passenger serverd %d\n", passenger.id     );
            pthread_mutex_unlock(&queue2_mutex);
            pthread_mutex_unlock(&q2_status_mutex);

        }
        // serve passenger





    }

    free(clerk_id);

}


    void *passenger_handler(void *p_info) {
        struct customer *passenger = (struct customer *) p_info;




        if (passenger->class == 1) {

            pthread_mutex_lock(&queue1_mutex);
            enqueue(q1, passenger->id);
            q1_len++;
            pthread_mutex_unlock(&queue1_mutex);


        } else {
            pthread_mutex_lock(&queue2_mutex);
            enqueue(q2, passenger->id);
            q2_len++;
            pthread_mutex_unlock(&queue2_mutex);



        }
        usleep(passenger->arrival_time * 100000);
        fprintf(stdout, "A customer arrives: customer ID %2d. \n", passenger->id);

        //add time start clock

        // got to check which clerk signaled me

        /*
        if (passenger->class == 1) {
            pthread_mutex_lock(&queue1_mutex);
            while (front(q1) != passenger->id) {
                pthread_cond_wait(&queue1_cv, &queue1_mutex);


            }
            if (passenger->id == front(q1)) {
                usleep(passenger->service_time);
            }
            pthread_mutex_unlock(&queue1_mutex);
        }
        else {
            pthread_mutex_lock(&queue1_mutex);
            while (front(q2) != passenger->id) {

                pthread_cond_wait(&queue2_cv, &clerk_sig_mutex);

            }
            pthread_mutex_unlock(&queue2_mutex);
        }
        */
        // wait for clerk_signal from clerk


        pthread_exit(NULL);
    }


    void read_file(FILE *file, int size, struct customer p_list[]) {


        int id, class, a_time, s_time;
        // Read and discard a line
        for (int i = 0; i < size; i++) {


            //copy the passenger info into a struct
            fscanf(file, "%d:%d,%d,%d", &id, &class, &a_time, &s_time);
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
        char *filename = argv[1];
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            printf("unable to read file \n");

        }

        char buff[200];
        fgets(buff, 200, file);
        size = atoi(buff); //get num of passengers and remove first line
        count = 0;
        q1 = createQueue(size);
        q2 = createQueue(size);
        //struct customer passenger_list[size];
        read_file(file, size, passenger_list); // add all passengers to a list

        for (int i = 0; i < size; ++i) {
            //printf("%d/n",i);
            printf("%d,%d,%d,%d\n", passenger_list[i].id, passenger_list[i].class, passenger_list[i].arrival_time,
                   passenger_list[i].service_time);

        }


        if (pthread_mutex_init(&clerk_sig_mutex, NULL)) {
            printf("clerk_handler mutex initialization error\n");
            exit(0);
        }
        if (pthread_mutex_init(&q1_status_mutex, NULL)) {
            printf("clerk_handler mutex initialization error\n");
            exit(0);
        }
        if (pthread_mutex_init(&q2_status_mutex, NULL)) {
            printf("clerk_handler mutex initialization error\n");
            exit(0);
        }

        // initialize queue mutexes
        if (pthread_mutex_init(&queue1_mutex, NULL)) {
            printf("queue mutex initialization error\n");
            exit(0);
        }
        if (pthread_mutex_init(&queue2_mutex, NULL)) {
            printf("queue mutex initialization error\n");
            exit(0);
        }

        // time mutex
        if (pthread_mutex_init(&time_mutex, NULL)) {
            printf("Error initializing time mutex.\n");
            exit(0);
        }
        // clerk_handler cv


        if (pthread_cond_init(&clerk_sig_cv, NULL)) {
            printf("Error at clerk_handler 2 cv\n");
            exit(0);
        }



        // create clerks
        pthread_t clerk_id[5];

        for (int i = 0; i < 5; ++i) {

            int* temp =  malloc(sizeof(int));
            *temp = i;

            if (pthread_create(&clerk_id[i], NULL, &clerk_handler, temp )!=0 ) {
                printf("can't create clerk_handler thread %d\n", i);
                exit(0);
            }


        }


        // create passengers

        pthread_t passenger_id[size];
        for (int i = 0; i < size; ++i) {
            if (pthread_create(&passenger_id[i], NULL, passenger_handler, (void *) &passenger_list[i]) != 0) {
                printf("can't create passenger thread %d\n", i);
            }

        }

        // calculate waiting time
        for (int i = 0; i < size ; ++i) {
            if(pthread_join(passenger_id[i], NULL)){
                printf("cannot join thread.\n");
                exit(0);
            }

        }

        for (int i = 0; i < 5 ; ++i) {
            if(pthread_join(clerk_id[i], NULL)!=0){
                printf("cannot join thread.\n");
                exit(0);
            }

        }



        pthread_mutex_destroy(&clerk_sig_mutex);
        pthread_mutex_destroy(&queue1_mutex);
        pthread_mutex_destroy(&queue2_mutex);
        pthread_mutex_destroy(&q1_status_mutex);
        pthread_mutex_destroy(&q2_status_mutex);
        pthread_mutex_destroy(&time_mutex);
        pthread_mutex_destroy(&q1_status_mutex);
        pthread_cond_destroy(&clerk_sig_cv);
        return 0;
        //@todo
        //add passenger list to two queues
        // add join to terminate threads


    }


