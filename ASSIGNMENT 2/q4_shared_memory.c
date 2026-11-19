#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct SharedData {
    int readings[6];
    int count;
    int sum;
    int maximum;
};

int main(void)
{
    struct SharedData *shared = mmap(NULL, sizeof(*shared),
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    shared->count = 0;
    shared->sum = 0;
    shared->maximum = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(shared, sizeof(*shared));
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        const int input[] = {31, 33, 36, 34, 41, 38};
        for (int i = 0; i < 6; i++)
            shared->readings[i] = input[i];
        shared->count = 6;
        _exit(EXIT_SUCCESS);
    }

    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        munmap(shared, sizeof(*shared));
        return EXIT_FAILURE;
    }

    for (int i = 0; i < shared->count; i++) {
        shared->sum += shared->readings[i];
        if (i == 0 || shared->readings[i] > shared->maximum)
            shared->maximum = shared->readings[i];
    }

    printf("shared memory: %d readings, average %.1f, max %d\n",
           shared->count,
           shared->count ? (double)shared->sum / shared->count : 0.0,
           shared->maximum);

    munmap(shared, sizeof(*shared));
    return EXIT_SUCCESS;
}
