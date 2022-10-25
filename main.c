#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>
#include "queue.h"



struct customer{ //use this struct to record the customer information read from customers.txt
    int user_id;
    int service_time;
    int arrival_time;
    int class;
    double start;
    double end;

};

void read_file(FILE *file, int size, struct customer p_list[] );

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
        printf("%d,%d,%d,%d\n",passenger_list[i].user_id,passenger_list[i].class, passenger_list[i].arrival_time, passenger_list[i].service_time);

    }
    return 0;
    //add passenger list to two queues
    // setup 5 clerks to process these
    // setup mutex locks
    // setup convar

}



void read_file(FILE *file, int size, struct customer p_list[]) {




    int id, class, a_time, s_time;
    // Read and discard a line
    for (int i = 0; i < size; i++) {


        //copy the passenger info into a struct
        fscanf(file, "%d:%d,%d,%d",&id,&class,&a_time,&s_time);
        p_list[i].user_id = id;
        p_list[i].class = class;
        p_list[i].arrival_time = a_time;
        p_list[i].service_time = s_time;
        //printf("%d,%d, %d, %d\n",p_list[i].user_id,p_list[i].class, p_list[i].arrival_time, p_list[i].service_time);

    }


    fclose(file);
}






