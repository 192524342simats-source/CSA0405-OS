#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("Child process: PID = %ld, Parent PID = %ld\n",
               (long)getpid(), (long)getppid());
        return EXIT_SUCCESS;
    }

    printf("Parent process: PID = %ld, Child PID = %ld\n",
           (long)getpid(), (long)pid);

    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    printf("Parent: child process completed\n");
    return EXIT_SUCCESS;
}
