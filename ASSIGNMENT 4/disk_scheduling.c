/* Task 4: FCFS, SSTF, SCAN and C-SCAN for the assignment's request queue */
#include <stdio.h>
#include <stdlib.h>

#define N 8
#define MAXCYL 199
static int requests[N] = {98, 183, 37, 122, 14, 124, 65, 67};

static int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static int movement(const int order[], int n, int head) {
    int total = 0, pos = head;
    for (int i = 0; i < n; ++i) { total += abs(order[i] - pos); pos = order[i]; }
    return total;
}

static void show(const char *name, const int order[], int n, int head) {
    printf("%-7s %d", name, head);
    for (int i = 0; i < n; ++i) printf(" %d", order[i]);
    printf(" -> %d cylinders\n", movement(order, n, head));
}

int main(void) {
    int order[N + 2], sorted[N];
    printf("=== DISK SCHEDULING (HEAD = 53) ===\n");
    show("FCFS", requests, N, 53);

    int done[N] = {0}, pos = 53;
    for (int k = 0; k < N; ++k) {
        int best = -1, best_distance = MAXCYL + 1;
        for (int i = 0; i < N; ++i) if (!done[i]) {
            int d = abs(requests[i] - pos);
            if (d < best_distance) { best_distance = d; best = i; }
        }
        done[best] = 1; order[k] = requests[best]; pos = requests[best];
    }
    show("SSTF", order, N, 53);

    for (int i = 0; i < N; ++i) sorted[i] = requests[i];
    qsort(sorted, N, sizeof sorted[0], cmp_int);
    int n = 0;
    for (int i = N - 1; i >= 0; --i) if (sorted[i] < 53) order[n++] = sorted[i];
    order[n++] = 0;
    for (int i = 0; i < N; ++i) if (sorted[i] > 53) order[n++] = sorted[i];
    show("SCAN", order, n, 53);

    n = 0;
    for (int i = 0; i < N; ++i) if (sorted[i] > 53) order[n++] = sorted[i];
    order[n++] = MAXCYL;
    int first_leg = movement(order, n, 53);
    int low[N], m = 0;
    for (int i = 0; i < N; ++i) if (sorted[i] < 53) low[m++] = sorted[i];
    int second_leg = 0, p = 0;
    for (int i = 0; i < m; ++i) { second_leg += abs(low[i] - p); p = low[i]; }
    printf("C-SCAN  53 ... 199, jump to 0 ... 37 -> %d cylinders (+199 for the jump)\n",
           first_leg + second_leg);
    printf("Note: C-SCAN total shown excludes the circular jump; jump distance is reported separately.\n");
    return 0;
}
