/*
 * main.c
 * Big Challenge: Bag of Words - Top-K Word Frequency Analyzer
 * Menu utama, output, dan pengukuran waktu.
 *
 * Kompilasi:
 *   gcc -O2 -o BigChallenge main.c loader.c sort.c
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Big Challenge.h"
 
/* ===================== TIMER ===================== */
 
static double elapsed_ms(struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec  - start->tv_sec)  * 1000.0
         + (double)(end->tv_nsec - start->tv_nsec) / 1e6;
}
 
/* ===================== OUTPUT ===================== */
 
static void write_output(const char *filename, WordFreq *arr, long n,
                         double ms, const char *method) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "ERROR: Tidak bisa membuat file output: %s\n", filename);
        return;
    }
    fprintf(fp, "Metode pengurutan: %s\n\n", method);
    for (long i = 0; i < n; i++)
        fprintf(fp, "%s (%ld)\n", arr[i].word, arr[i].freq);
    fprintf(fp, "\nWaktu untuk mengurutkan: %.0f ms\n", ms);
    fclose(fp);
    printf("Output disimpan ke: %s\n", filename);
}
 
static void print_top_k(WordFreq *arr, long k) {
    printf("\n%-5s  %-30s  %s\n", "No.", "Kata", "Frekuensi");
    printf("%-5s  %-30s  %s\n", "---", "------------------------------", "---------");
    for (long i = 0; i < k; i++)
        printf("%-5ld  %-30s  %ld\n", i+1, arr[i].word, arr[i].freq);
}
 
/* ===================== MAIN ===================== */
 
int main(void) {
    char docword_path[1024];
    char vocab_path[1024];
 
    printf("=========================================\n");
    printf("  Big Challenge: Bag of Words Analyzer   \n");
    printf("=========================================\n\n");
 
    printf("Tentukan file docword: ");
    if (!fgets(docword_path, sizeof(docword_path), stdin)) return 1;
    unsigned long dlen = strlen(docword_path);
    if (dlen > 0 && docword_path[dlen-1] == '\n') docword_path[--dlen] = '\0';
 
    /* Muat docword */
    printf("\nMemuat file docword: %s\n", docword_path);
    if (!load_docword(docword_path)) return 1;
 
    /* Tentukan & muat vocabulary */
    derive_vocab_path(docword_path, vocab_path, sizeof(vocab_path));
    printf("File vocabulary: %s\n", vocab_path);
 
    char **vocab_words = NULL;
    long   vocab_count = load_vocabulary(vocab_path, &vocab_words);
    if (vocab_count <= 0) {
        fprintf(stderr, "ERROR: Gagal memuat vocabulary.\n");
        free(g_freq);
        return 1;
    }
    printf("  Vocabulary dimuat: %ld kata\n", vocab_count);
 
    /* Bangun array WordFreq */
    build_wf_array(vocab_words, vocab_count);
 
    /* ==================== MENU LOOP ==================== */
    int choice = 0;
    do {
        printf("\n-----------------------------------------\n");
        printf("Pilihan:\n");
        printf("  1) Urutkan dengan metode sederhana (Bubble Sort)\n");
        printf("  2) Urutkan dengan Quick Sort\n");
        printf("  3) Urutkan dengan Heap Sort\n");
        printf("  4) Tampilkan k kata terbanyak ke layar\n");
        printf("  5) Selesai\n");
        printf("-----------------------------------------\n");
        printf("Pilihan anda: ");
        if (scanf("%d", &choice) != 1) break;
        getchar(); /* consume newline */
 
        if (choice == 1 || choice == 2 || choice == 3) {
            const char *method_name =
                choice == 1 ? "Bubble Sort" :
                choice == 2 ? "Quick Sort"  : "Heap Sort";
            const char *default_suffix =
                choice == 1 ? "bubblesort" :
                choice == 2 ? "quicksort"  : "heapsort";
 
            char outfile[512];
            printf("Nama file output (Enter = output_%s.txt): ", default_suffix);
            if (fgets(outfile, sizeof(outfile), stdin)) {
                unsigned long olen = strlen(outfile);
                if (olen > 0 && outfile[olen-1] == '\n') outfile[--olen] = '\0';
                if (olen == 0)
                    snprintf(outfile, sizeof(outfile), "output_%s.txt", default_suffix);
            }
 
            WordFreq *arr = copy_wf();
            if (!arr) { fprintf(stderr, "ERROR: copy gagal\n"); break; }
 
            struct timespec t0, t1;
            clock_gettime(CLOCK_MONOTONIC, &t0);
 
            if      (choice == 1) bubble_sort(arr, g_wf_len);
            else if (choice == 2) quick_sort(arr, 0, g_wf_len - 1);
            else                  heap_sort(arr, g_wf_len);
 
            clock_gettime(CLOCK_MONOTONIC, &t1);
            double ms = elapsed_ms(&t0, &t1);
 
            printf("Waktu untuk mengurutkan: %.0f ms\n", ms);
            write_output(outfile, arr, g_wf_len, ms, method_name);
            free(arr);
 
        } else if (choice == 4) {
            long k;
            printf("Masukkan nilai k (10 < k < 150): ");
            if (scanf("%ld", &k) != 1) { getchar(); continue; }
            getchar();
            if (k <= 10 || k >= 150) {
                printf("PERINGATAN: k harus antara 10 dan 150.\n");
                continue;
            }
            if (k > g_wf_len) k = g_wf_len;
 
            WordFreq *arr = copy_wf();
            if (!arr) { fprintf(stderr, "ERROR: copy gagal\n"); break; }
 
            struct timespec t0, t1;
            clock_gettime(CLOCK_MONOTONIC, &t0);
            quick_sort(arr, 0, g_wf_len - 1);
            clock_gettime(CLOCK_MONOTONIC, &t1);
            double ms = elapsed_ms(&t0, &t1);
 
            print_top_k(arr, k);
            printf("\nWaktu untuk mengurutkan: %.0f ms\n", ms);
            free(arr);
 
        } else if (choice == 5) {
            printf("Program selesai. Terima kasih!\n");
        } else {
            printf("Pilihan tidak valid. Silakan pilih 1-5.\n");
        }
 
    } while (choice != 5);
 
    /* Cleanup */
    for (long i = 0; i < vocab_count; i++) free(vocab_words[i]);
    free(vocab_words);
    free(g_freq);
    free(g_wf);
 
    return 0;
}