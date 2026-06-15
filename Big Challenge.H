#ifndef BIGCHALLENGE_H
#define BIGCHALLENGE_H
 
/* ===================== DATA STRUCTURES ===================== */
 
typedef struct {
    char  *word;    /* kata dari vocabulary */
    long   freq;    /* total frekuensi di seluruh dokumen */
} WordFreq;
 
/* Global arrays (didefinisikan di loader.c) */
extern long     *g_freq;    /* freq[wordID-1] = total count */
extern long      g_W;       /* jumlah kata unik (vocabulary size) */
extern WordFreq *g_wf;      /* array WordFreq untuk sorting */
extern long      g_wf_len;  /* panjang array g_wf (kata yang freq > 0) */
 
/* ===================== loader.c ===================== */
void derive_vocab_path(const char *docword_path, char *vocab_path, unsigned long sz);
long load_vocabulary(const char *vocab_path, char ***out_words);
int  load_docword(const char *path);
void build_wf_array(char **vocab_words, long vocab_count);
WordFreq *copy_wf(void);
 
/* ===================== sort.c ===================== */
void bubble_sort(WordFreq *arr, long n);
void quick_sort(WordFreq *arr, long lo, long hi);
void heap_sort(WordFreq *arr, long n);
 
#endif /* BIGCHALLENGE_H */