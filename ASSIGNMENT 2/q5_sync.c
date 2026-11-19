#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 5
#define ITEMS 20

static int buffer[SIZE];
static int in = 0, out = 0;
static sem_t empty_slots, full_slots;
static pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;
static long consumed_sum = 0;

static void *producer(void *arg)
{
    (void)arg;
    for (int item = 1; item <= ITEMS; item++) {
        if (sem_wait(&empty_slots) != 0) {
            perror("sem_wait");
            return NULL;
        }
        pthread_mutex_lock(&buffer_lock);
        buffer[in] = item;
        in = (in + 1) % SIZE;
        pthread_mutex_unlock(&buffer_lock);
        sem_post(&full_slots);
    }
    return NULL;
}

static void *consumer(void *arg)
{
    (void)arg;
    for (int k = 0; k < ITEMS; k++) {
        if (sem_wait(&full_slots) != 0) {
            perror("sem_wait");
            return NULL;
        }
        pthread_mutex_lock(&buffer_lock);
        int item = buffer[out];
        out = (out + 1) % SIZE;
        pthread_mutex_unlock(&buffer_lock);
        sem_post(&empty_slots);

        pthread_mutex_lock(&buffer_lock);
        consumed_sum += item;
        pthread_mutex_unlock(&buffer_lock);
    }
    return NULL;
}

int main(void)
{
    if (sem_init(&empty_slots, 0, SIZE) != 0 ||
        sem_init(&full_slots, 0, 0) != 0) {
        perror("sem_init");
        return EXIT_FAILURE;
    }

    pthread_t p, c;
    if (pthread_create(&p, NULL, producer, NULL) != 0 ||
        pthread_create(&c, NULL, consumer, NULL) != 0) {
        fprintf(stderr, "Unable to create threads\n");
        return EXIT_FAILURE;
    }

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    printf("items consumed: %d, sum = %ld (expected %d)\n",
           ITEMS, consumed_sum, ITEMS * (ITEMS + 1) / 2);

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&buffer_lock);
    return consumed_sum == ITEMS * (ITEMS + 1) / 2 ? EXIT_SUCCESS : EXIT_FAILURE;
}
