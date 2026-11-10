#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

double nowMs() {
#ifdef _WIN32
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return t.QuadPart * 1000.0 / f.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1000000.0;
#endif
}

long long comparisons;
int depth, maxDepth;

void mergeSort(int a[], int temp[], int left, int right) {
    depth++;
    if (depth > maxDepth) maxDepth = depth;

    if (left < right) {
        int mid = (left + right) / 2;
        mergeSort(a, temp, left, mid);
        mergeSort(a, temp, mid + 1, right);

        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            comparisons++;
            if (a[i] <= a[j]) temp[k++] = a[i++];
            else temp[k++] = a[j++];
        }
        while (i <= mid) temp[k++] = a[i++];
        while (j <= right) temp[k++] = a[j++];
        for (i = left; i <= right; i++) a[i] = temp[i];
    }
    depth--;
}

void swap(int* x, int* y) {
    int t = *x;
    *x = *y;
    *y = t;
}

int medianIndex(int a[], int low, int high) {
    int mid = (low + high) / 2;
    comparisons++;
    if (a[low] < a[mid]) {
        comparisons++;
        if (a[mid] < a[high]) return mid;
        comparisons++;
        if (a[low] < a[high]) return high;
        return low;
    }
    comparisons++;
    if (a[low] < a[high]) return low;
    comparisons++;
    if (a[mid] < a[high]) return high;
    return mid;
}

void quickSort(int a[], int low, int high, int mode) {
    if (low >= high) return;
    depth++;
    if (depth > maxDepth) maxDepth = depth;

    int p = low;
    if (mode == 1) p = low + rand() % (high - low + 1);
    if (mode == 2) p = medianIndex(a, low, high);
    swap(&a[low], &a[p]);

    int i = low + 1;
    for (int j = low + 1; j <= high; j++) {
        comparisons++;
        if (a[j] < a[low]) swap(&a[i++], &a[j]);
    }
    swap(&a[low], &a[i - 1]);
    quickSort(a, low, i - 2, mode);
    quickSort(a, i, high, mode);
    depth--;
}

void test(char* name, int original[], int n, int type) {
    int* a = malloc(n * sizeof(int));
    int* temp = type == 0 ? malloc(n * sizeof(int)) : NULL;
    if (!a || (type == 0 && !temp)) {
        printf("Memory allocation failed\n");
        free(a); free(temp);
        exit(1);
    }
    for (int i = 0; i < n; i++) a[i] = original[i];

    comparisons = depth = maxDepth = 0;
    double start = nowMs();
    if (type == 0) mergeSort(a, temp, 0, n - 1);
    else quickSort(a, 0, n - 1, type - 1);
    double ms = nowMs() - start;

    long long memory = (long long)maxDepth * (type == 0 ? 40 : 48);
    if (type == 0) memory += (long long)n * sizeof(int);

    int sorted = 1;
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i]) sorted = 0;

    printf("%-16s %-14lld %-11.6f %lld", name, comparisons, ms, memory);
    if (!sorted) printf("  Sorting Error!");
    printf("\n");
    free(temp);
    free(a);
}

int main() {
    int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    srand(12345);

    for (int s = 0; s < 6; s++) {
        int n = sizes[s];
        int* original = malloc(n * sizeof(int));
        if (!original) return 1;
        for (int i = 0; i < n; i++) original[i] = rand() % 100000;

        printf("Data Size : %d\n\n", n);
        printf("Algorithm        Comparisons    Time(ms)    Memory(Bytes)\n");
        printf("---------------------------------------------------------\n");
        test("Merge Sort", original, n, 0);
        test("Quick (First)", original, n, 1);
        test("Quick (Random)", original, n, 2);
        test("Quick (Median3)", original, n, 3);
        printf("\n");
        free(original);
    }
    return 0;
}
