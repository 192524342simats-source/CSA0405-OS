#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CAMERAS 64
#define WORK_PER_CAMERA 250000

static int next_camera = 0;
static pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
static volatile unsigned long long checksum = 0;
static pthread_mutex_t sum_lock = PTHREAD_MUTEX_INITIALIZER;

static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void process_camera(int camera_id)
{
    unsigned long long local = (unsigned long long)camera_id + 1;
    for (int i = 0; i < WORK_PER_CAMERA; i++)
        local = (local * 1664525ULL + 1013904223ULL) % 4294967291ULL;

    pthread_mutex_lock(&sum_lock);
    checksum += local;
    pthread_mutex_unlock(&sum_lock);
}

static void *camera_worker(void *arg)
{
    (void)arg;
    for (;;) {
        pthread_mutex_lock(&queue_lock);
        int camera = next_camera++;
        pthread_mutex_unlock(&queue_lock);

        if (camera >= CAMERAS)
            break;
        process_camera(camera);
    }
    return NULL;
}

static double run_pool(int thread_count)
{
    pthread_t *threads = calloc((size_t)thread_count, sizeof(*threads));
    if (!threads) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    next_camera = 0;
    checksum = 0;
    double start = now_seconds();

    for (int i = 0; i < thread_count; i++) {
        int rc = pthread_create(&threads[i], NULL, camera_worker, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed (error %d)\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < thread_count; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed (error %d)\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    double elapsed = now_seconds() - start;
    free(threads);
    return elapsed;
}

int main(void)
{
    const int thread_counts[] = {1, 4, 8, 16};
    double baseline = 0.0;

    puts("Traffic-camera workload: 64 cameras");
    puts("Threads\tExecution time (s)\tSpeed-up");

    for (size_t i = 0; i < sizeof(thread_counts) / sizeof(thread_counts[0]); i++) {
        double elapsed = run_pool(thread_counts[i]);
        if (i == 0)
            baseline = elapsed;
        printf("%d\t%.6f\t\t%.2f\n",
               thread_counts[i], elapsed, baseline / elapsed);
    }

    printf("Checksum: %llu\n", checksum);
    pthread_mutex_destroy(&queue_lock);
    pthread_mutex_destroy(&sum_lock);
    return 0;
}
