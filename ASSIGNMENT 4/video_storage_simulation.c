/* Task 6: Reproducible workload model comparing HDD-only and SSD/cache designs.
   Latencies are explicit simulation assumptions, not physical benchmark results. */
#include <stdio.h>
#include <stdlib.h>

#define REQUESTS 10000
#define STUDENTS 200000
#define VIDEO_COUNT 1000
#define AVG_VIDEO_MB 1024.0
#define CHUNK_MB 4.0
#define SEED 424242u

static unsigned rng_state = SEED;
static unsigned next_random(void) {
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}
static int cmp_double(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

static void report(const char *label, double samples[], double total_ms) {
    double sorted[REQUESTS];
    for (int i = 0; i < REQUESTS; ++i) sorted[i] = samples[i];
    qsort(sorted, REQUESTS, sizeof sorted[0], cmp_double);
    printf("%-20s mean latency: %7.3f ms | p95: %7.3f ms | throughput: %8.1f req/s\n",
           label, total_ms / REQUESTS, sorted[(REQUESTS * 95) / 100],
           1000.0 * REQUESTS / total_ms);
}

int main(void) {
    double hdd[REQUESTS], hybrid[REQUESTS];
    int cache_hits = 0;
    const double hdd_seek_ms = 8.0, hdd_transfer_ms = 0.50;
    const double ssd_read_ms = 0.80, cache_read_ms = 0.15;
    double hdd_total = 0.0, hybrid_total = 0.0;

    for (int i = 0; i < REQUESTS; ++i) {
        (void)(next_random() % VIDEO_COUNT); /* video selection */
        (void)(next_random() % 250);         /* random 4 MB chunk within a 1 GB average video */
        hdd[i] = hdd_seek_ms + hdd_transfer_ms;
        /* Model a warm edge/server cache hit rate of 80%; misses read from SSD. */
        if ((next_random() % 100) < 80) { hybrid[i] = cache_read_ms; ++cache_hits; }
        else hybrid[i] = ssd_read_ms;
        hdd_total += hdd[i];
        hybrid_total += hybrid[i];
    }

    printf("=== VIDEO STREAMING STORAGE WORKLOAD MODEL ===\n");
    printf("Students served: %d | Synthetic requests: %d | RNG seed: %u\n", STUDENTS, REQUESTS, SEED);
    printf("Average video size assumption: %.0f MB | Chunk size: %.0f MB\n", AVG_VIDEO_MB, CHUNK_MB);
    printf("Capacity for %d average videos: %.2f TB raw; %.2f TB with 3 replicas\n",
           VIDEO_COUNT, VIDEO_COUNT * AVG_VIDEO_MB / 1000000.0,
           3.0 * VIDEO_COUNT * AVG_VIDEO_MB / 1000000.0);
    printf("Peak bandwidth planning example: 200,000 x 2 Mbps = 400 Gbps\n");
    printf("Indexed/extent lookup model: 1 metadata read + 1 data read; cached metadata: 1 data read\n");
    printf("\nSimulation assumptions: HDD seek 8.00 ms + transfer 0.50 ms; SSD 0.80 ms; cache 0.15 ms; cache hit 80%%\n");
    report("Design A: HDD only", hdd, hdd_total);
    report("Design B: SSD + cache", hybrid, hybrid_total);
    printf("Cache hits observed: %d / %d (%.1f%%)\n", cache_hits, REQUESTS,
           100.0 * cache_hits / REQUESTS);
    printf("Interpretation: the hybrid model reduces random-read latency; production results require fio/load testing on target hardware.\n");
    return 0;
}
