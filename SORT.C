/*
 * sort.c
 * Implementasi tiga algoritma pengurutan secara descending:
 *   - Bubble Sort
 *   - Quick Sort  (iteratif, aman untuk dataset besar)
 *   - Heap Sort
*/

#include <stdlib.h>
#include "Big Challenge.h"
 
/* ===================== BUBBLE SORT ===================== */
 
void bubble_sort(WordFreq *arr, long n) {
    for (long i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (long j = 0; j < n - i - 1; j++) {
            if (arr[j].freq < arr[j+1].freq) {
                WordFreq tmp = arr[j];
                arr[j]       = arr[j+1];
                arr[j+1]     = tmp;
                swapped = 1;
            }
        }
        if (!swapped) break;  /* early-exit jika sudah terurut */
    }
}
 
/* ===================== QUICK SORT ===================== */
 
static long qs_partition(WordFreq *arr, long lo, long hi) {
    long pivot = arr[hi].freq;
    long i     = lo - 1;
    for (long j = lo; j < hi; j++) {
        if (arr[j].freq >= pivot) {  /* descending */
            i++;
            WordFreq tmp = arr[i];
            arr[i]       = arr[j];
            arr[j]       = tmp;
        }
    }
    WordFreq tmp = arr[i+1];
    arr[i+1]     = arr[hi];
    arr[hi]      = tmp;
    return i + 1;
}
 
/* Versi iteratif menggunakan stack eksplisit agar tidak stack overflow
   pada dataset besar seperti PubMed (141k kata). */
void quick_sort(WordFreq *arr, long lo, long hi) {
    long *stack = (long *)malloc(2 * (unsigned long)(hi - lo + 2) * sizeof(long));
    if (!stack) return;
    long top = -1;
    stack[++top] = lo;
    stack[++top] = hi;
    while (top >= 0) {
        long h = stack[top--];
        long l = stack[top--];
        if (l < h) {
            long p = qs_partition(arr, l, h);
            if (p - 1 > l) { stack[++top] = l; stack[++top] = p - 1; }
            if (p + 1 < h) { stack[++top] = p + 1; stack[++top] = h; }
        }
    }
    free(stack);
}
 
/* ===================== HEAP SORT ===================== */
 
static void hs_heapify(WordFreq *arr, long n, long i) {
    long largest = i, l = 2*i+1, r = 2*i+2;
    if (l < n && arr[l].freq > arr[largest].freq) largest = l;
    if (r < n && arr[r].freq > arr[largest].freq) largest = r;
    if (largest != i) {
        WordFreq tmp = arr[i];
        arr[i]       = arr[largest];
        arr[largest] = tmp;
        hs_heapify(arr, n, largest);
    }
}
 
/* Max-heap → hasil ascending → dibalik jadi descending */
void heap_sort(WordFreq *arr, long n) {
    /* Build max-heap */
    for (long i = n/2 - 1; i >= 0; i--)
        hs_heapify(arr, n, i);
 
    /* Extract ascending */
    for (long i = n - 1; i > 0; i--) {
        WordFreq tmp = arr[0];
        arr[0]       = arr[i];
        arr[i]       = tmp;
        hs_heapify(arr, i, 0);
    }
 
    /* Reverse → descending */
    for (long i = 0, j = n-1; i < j; i++, j--) {
        WordFreq tmp = arr[i];
        arr[i]       = arr[j];
        arr[j]       = tmp;
    }
}