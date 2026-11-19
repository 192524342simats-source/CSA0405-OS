#include <stdio.h>

#define N 4
#define QUEUE_CAPACITY 128

int main(void)
{
    const char *name[N] = {"P1", "P2", "P3", "P4"};
    const int arrival[N] = {0, 1, 2, 3};
    const int burst[N] = {8, 4, 9, 5};
    const int quantum = 3;

    int remaining[N], finish[N] = {0};
    int queue[QUEUE_CAPACITY];
    int added[N] = {0};
    int head = 0, tail = 0;
    int time = 0, completed = 0;

    for (int i = 0; i < N; i++)
        remaining[i] = burst[i];

    /* Start with processes that have arrived at time zero. */
    for (int i = 0; i < N; i++) {
        if (arrival[i] <= time && !added[i]) {
            queue[tail++] = i;
            added[i] = 1;
        }
    }

    printf("Gantt:");
    while (completed < N) {
        if (head == tail) {
            time++;
            for (int i = 0; i < N; i++) {
                if (!added[i] && arrival[i] <= time) {
                    queue[tail++] = i;
                    added[i] = 1;
                }
            }
            continue;
        }

        int p = queue[head++];
        int run = remaining[p] < quantum ? remaining[p] : quantum;
        int start = time;
        time += run;
        remaining[p] -= run;
        printf(" %s(%d-%d)", name[p], start, time);

        /* Add newly arrived processes before re-queuing this process. */
        for (int i = 0; i < N; i++) {
            if (!added[i] && arrival[i] <= time) {
                queue[tail++] = i;
                added[i] = 1;
            }
        }

        if (remaining[p] > 0) {
            queue[tail++] = p;
        } else {
            finish[p] = time;
            completed++;
        }
    }

    double total_tat = 0.0, total_wt = 0.0;
    printf("\n\nProcess  Arrival  Burst  Completion  TAT  WT\n");
    for (int i = 0; i < N; i++) {
        int tat = finish[i] - arrival[i];
        int wt = tat - burst[i];
        total_tat += tat;
        total_wt += wt;
        printf("%-7s  %7d  %5d  %10d  %3d  %2d\n",
               name[i], arrival[i], burst[i], finish[i], tat, wt);
    }

    printf("\nAverage TAT = %.2f\n", total_tat / N);
    printf("Average WT  = %.2f\n", total_wt / N);
    return 0;
}
