/*
 * loader.c
 * Fungsi-fungsi untuk membaca file docword.*.txt dan vocab.*.txt,
 * serta membangun struktur data WordFreq.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Big Challenge.h"
 
/* ===================== GLOBALS ===================== */
 
long     *g_freq   = NULL;
long      g_W      = 0;
WordFreq *g_wf     = NULL;
long      g_wf_len = 0;
 
/* ===================== VOCABULARY ===================== */
 
/*
 * Derivasi nama file vocab dari nama file docword.
 * docword.kos.txt  --> vocab.kos.txt
 */
void derive_vocab_path(const char *docword_path, char *vocab_path, unsigned long sz) {
    const char *base = strrchr(docword_path, '/');
    if (!base) base = strrchr(docword_path, '\\');
    const char *fname = base ? base + 1 : docword_path;
 
    if (strncmp(fname, "docword.", 8) == 0) {
        unsigned long dir_len = (unsigned long)(fname - docword_path);
        char   dir[1024] = {0};
        if (dir_len > 0 && dir_len < sizeof(dir))
            strncpy(dir, docword_path, dir_len);
        snprintf(vocab_path, sz, "%svocab.%s", dir, fname + 8);
    } else {
        printf("Tidak bisa menentukan nama vocab otomatis.\n");
        printf("Masukkan path file vocabulary: ");
        if (fgets(vocab_path, (int)sz, stdin)) {
            unsigned long n = strlen(vocab_path);
            if (n > 0 && vocab_path[n-1] == '\n') vocab_path[n-1] = '\0';
        }
    }
}
 
/* Muat vocabulary ke dalam array string. Return jumlah kata, atau -1 jika gagal. */
long load_vocabulary(const char *vocab_path, char ***out_words) {
    FILE *fp = fopen(vocab_path, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Tidak bisa membuka file vocabulary: %s\n", vocab_path);
        return -1;
    }
 
    long count = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) count++;
    rewind(fp);
 
    char **words = (char **)malloc((unsigned long)count * sizeof(char *));
    if (!words) { fclose(fp); return -1; }
 
    long i = 0;
    while (i < count && fgets(line, sizeof(line), fp)) {
        unsigned long n = strlen(line);
        while (n > 0 && (line[n-1] == '\n' || line[n-1] == '\r')) { line[--n] = '\0'; }
        words[i] = (char *)malloc(n + 1);
        if (!words[i]) { fclose(fp); return -1; }
        strcpy(words[i], line);
        i++;
    }
    fclose(fp);
    *out_words = words;
    return count;
}
 
/* ===================== DOCWORD ===================== */
 
/*
 * Baca file docword.*.txt dan akumulasi frekuensi tiap kata ke g_freq[].
 * Baris 1-3: D W N
 * Baris 4+ : docID wordID count
 */
int load_docword(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Tidak bisa membuka file: %s\n", path);
        return 0;
    }
 
    long D, W, N;
    if (fscanf(fp, "%ld", &D) != 1 ||
        fscanf(fp, "%ld", &W) != 1 ||
        fscanf(fp, "%ld", &N) != 1) {
        fprintf(stderr, "ERROR: Format header tidak valid.\n");
        fclose(fp);
        return 0;
    }
    printf("  D=%ld  W=%ld  N=%ld\n", D, W, N);
 
    g_W    = W;
    g_freq = (long *)calloc((unsigned long)W, sizeof(long));
    if (!g_freq) {
        fprintf(stderr, "ERROR: Alokasi memori gagal (W=%ld).\n", W);
        fclose(fp);
        return 0;
    }
 
    long docID, wordID, count, read_count = 0;
    while (fscanf(fp, "%ld %ld %ld", &docID, &wordID, &count) == 3) {
        if (wordID >= 1 && wordID <= W)
            g_freq[wordID - 1] += count;
        read_count++;
        if (read_count % 5000000 == 0)
            printf("  Membaca %ld triple...\n", read_count);
    }
    printf("  Total triple NNZ dibaca: %ld\n", read_count);
    fclose(fp);
    return 1;
}
 
/* ===================== BUILD WordFreq ARRAY ===================== */
 
void build_wf_array(char **vocab_words, long vocab_count) {
    long cnt = 0;
    long lim = (g_W < vocab_count) ? g_W : vocab_count;
    for (long i = 0; i < lim; i++)
        if (g_freq[i] > 0) cnt++;
 
    g_wf = (WordFreq *)malloc((unsigned long)cnt * sizeof(WordFreq));
    if (!g_wf) { fprintf(stderr, "ERROR: malloc g_wf gagal\n"); return; }
 
    long j = 0;
    for (long i = 0; i < lim && j < cnt; i++) {
        if (g_freq[i] > 0) {
            g_wf[j].word = vocab_words[i];
            g_wf[j].freq = g_freq[i];
            j++;
        }
    }
    g_wf_len = j;
    printf("  Kata unik dengan frekuensi > 0: %ld\n", g_wf_len);
}
 
/* Salin g_wf agar data asli tidak rusak saat sorting */
WordFreq *copy_wf(void) {
    WordFreq *cp = (WordFreq *)malloc((unsigned long)g_wf_len * sizeof(WordFreq));
    if (!cp) return NULL;
    memcpy(cp, g_wf, (unsigned long)g_wf_len * sizeof(WordFreq));
    return cp;
}