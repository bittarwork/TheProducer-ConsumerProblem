#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5 // Size of the buffer
#define NUM_PRODUCERS 2 // Number of producers
#define NUM_CONSUMERS 2 // Number of consumers
#define NUM_ITEMS 10 // Number of items produced by each producer

sem_t mutex; // Semaphore for controlling producer and consumer access to the buffer
sem_t full; // Semaphore indicating the number of items in the buffer
sem_t empty; // Semaphore indicating the number of empty slots in the buffer

int buffer[BUFFER_SIZE]; // The buffer
int count = 0; // Number of items in the buffer

// Producer procedure
void* producer(void* arg) {
    int producer_id = *(int*)arg;

    for (int i = 0; i < NUM_ITEMS; i++) {
        // Produce an item
        int item = rand() % 100;

        // Wait for an empty slot in the buffer
        sem_wait(&empty);

        // Acquire the mutex semaphore to access the buffer
        sem_wait(&mutex);

        // Put the item in the buffer
        buffer[count] = item;
        count++;

        printf("Producer %d produced item: %d\n", producer_id, item);

        // Release the mutex semaphore
        sem_post(&mutex);

        // Notify the full semaphore of a new item in the buffer
        sem_post(&full);
    }

    pthread_exit(NULL);
}

// Consumer procedure
void* consumer(void* arg) {
    int consumer_id = *(int*)arg;

    for (int i = 0; i < NUM_ITEMS; i++) {
        // Wait for an item in the buffer
        sem_wait(&full);

        // Acquire the mutex semaphore to access the buffer
        sem_wait(&mutex);

        // Consume an item from the buffer
        int item = buffer[count - 1];
        count--;

        printf("Consumer %d consumed item: %d\n", consumer_id, item);

        // Release the mutex semaphore
        sem_post(&mutex);

        // Notify the empty semaphore of an empty slot in the buffer
        sem_post(&empty);
    }

    pthread_exit(NULL);
}

int main() {
    // Initialize the semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    // Create producer and consumer threads
    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t consumer_threads[NUM_CONSUMERS];

    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    // Start execution
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producer_threads[i], NULL, producer, &producer_ids[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumer_threads[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for threads to finish execution
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    // Destroy the semaphores
    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    return 0;
}