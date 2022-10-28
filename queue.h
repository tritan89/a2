//
// Created by tlang on 21/10/2022.
//

#ifndef UNTITLED_QUEUE_H
#define UNTITLED_QUEUE_H
typedef struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
} Queue;
Queue* createQueue(unsigned capacity);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);

#endif //UNTITLED_QUEUE_H
