#include <stdio.h>
#include <string.h>

#define P 5
#define R 3

static int available[R] = {3, 3, 2};
static int maximum[P][R] = {
    {7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}
};
static int allocation[P][R] = {
    {0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}
};

static int is_safe(int sequence[P])
{
    int work[R];
    int finished[P] = {0};
    int count = 0;
    memcpy(work, available, sizeof(work));

    while (count < P) {
        int found = 0;
        for (int p = 0; p < P; p++) {
            if (finished[p])
                continue;

            int possible = 1;
            for (int r = 0; r < R; r++) {
                int need = maximum[p][r] - allocation[p][r];
                if (need > work[r]) {
                    possible = 0;
                    break;
                }
            }

            if (possible) {
                for (int r = 0; r < R; r++)
                    work[r] += allocation[p][r];
                finished[p] = 1;
                sequence[count++] = p;
                found = 1;
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}

static void request_resources(int p, const int request[R])
{
    for (int r = 0; r < R; r++) {
        int need = maximum[p][r] - allocation[p][r];
        if (request[r] > need || request[r] > available[r]) {
            printf("P%d request: must wait (not available or exceeds need)\n", p);
            return;
        }
    }

    for (int r = 0; r < R; r++) {
        available[r] -= request[r];
        allocation[p][r] += request[r];
    }

    int sequence[P];
    if (is_safe(sequence)) {
        printf("P%d request: granted, safe sequence", p);
        for (int i = 0; i < P; i++)
            printf(" P%d", sequence[i]);
        putchar('\n');
    } else {
        for (int r = 0; r < R; r++) {
            available[r] += request[r];
            allocation[p][r] -= request[r];
        }
        printf("P%d request: denied (unsafe)\n", p);
    }
}

int main(void)
{
    int sequence[P];
    if (is_safe(sequence)) {
        printf("initial state safe:");
        for (int i = 0; i < P; i++)
            printf(" P%d", sequence[i]);
        putchar('\n');
    } else {
        puts("initial state is unsafe");
        return 1;
    }

    request_resources(1, (int[R]){1, 0, 2});
    request_resources(4, (int[R]){3, 3, 0});
    request_resources(0, (int[R]){0, 2, 0});
    return 0;
}
