#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

// Senin "Request" yapına karşılık gelen yeni veri yapısı
typedef struct {
    int id;
    int priority_level;  // Senin urgency değerin
    bool is_fast_track;  // Senin incremental değerin
    char description[128];
} Element;

// Senin "MinHeap" yapına karşılık gelen ana yapı
typedef struct {
    Element *items;      // Dinamik dizi (array)
    int max_size;        // Kapasite
    int current_size;    // Mevcut doluluk
} PQueue;

// --- Fonksiyon Prototipleri ---

// Kuyruğu oluşturur (create_heap yerine)
PQueue* init_queue(int initial_capacity);

// Yeni eleman ekler (insert yerine)
void enqueue(PQueue *pq, Element el);

// En öncelikli elemanı çıkarır (extract_min yerine)
Element dequeue(PQueue *pq);

// Belleği temizler (free_heap yerine)
void destroy_queue(PQueue *pq);

// Kuyruk boş mu kontrolü
bool is_queue_empty(PQueue *pq);

#endif
