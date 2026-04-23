#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Veri Yapısı Tanımları ---
typedef struct {
    int task_id;
    int urgency;      // 0: En Yüksek, 7: En Düşük (RFC 9213 Standartı gibi)
    float cpu_load;   // Tahmini CPU yükü
    char service[32]; // Servis adı
} Task;

typedef struct {
    Task *data;       // Dinamik dizi
    int size;         // Mevcut eleman sayısı
    int capacity;     // Maksimum kapasite
} TaskQueue;

// --- Fonksiyon Protokolleri ---
TaskQueue* create_queue(int init_cap);
void push_task(TaskQueue *q, Task t);
Task pop_task(TaskQueue *q);
void swap(Task *a, Task *b);
void free_queue(TaskQueue *q);

// --- Ana Program ---
int main() {
    printf("--- Bulut Gorev Zamanlayicisi Baslatildi ---\n\n");

    // Başlangıç kapasitesi 2 (Realloc test etmek için düşük tutuldu)
    TaskQueue *scheduler = create_queue(2);

    // Örnek Görev Listesi
    Task incoming_tasks[] = {
        {101, 5, 0.2, "LogService"},
        {102, 0, 0.9, "AuthService"},    // En acil (Urgency 0)
        {103, 3, 0.5, "PaymentService"},
        {104, 7, 0.1, "CleanupService"}, // En düşük (Urgency 7)
        {105, 1, 0.8, "DatabaseWrite"}
    };

    int n = sizeof(incoming_tasks) / sizeof(incoming_tasks[0]);

    printf("[+] Gorevler kuyruga ekleniyor...\n");
    for (int i = 0; i < n; i++) {
        push_task(scheduler, incoming_tasks[i]);
        printf("  -> Eklendi: %-15s | Urgency: %d\n", incoming_tasks[i].service, incoming_tasks[i].urgency);
    }

    printf("\n--- Gorev Isleme Basliyor (Min-Heap Siralamasi) ---\n");
    printf("----------------------------------------------------------\n");

    // Kuyruk boşalana kadar en düşük öncelik değerine sahip (en acil) görevi çek
    while (scheduler->size > 0) {
        Task current = pop_task(scheduler);
        printf("ISLENIYOR -> [ID: %d] Servis: %-15s | Urgency: %d | Yuk: %% %.0f\n", 
                current.task_id, current.service, current.urgency, current.cpu_load * 100);
    }

    // Temizlik
    free_queue(scheduler);
    printf("\n[+] Sistem bellegi temizlendi. Kapaniliyor...\n");

    return 0;
}

// --- Fonksiyon Implementasyonları ---

TaskQueue* create_queue(int init_cap) {
    TaskQueue *q = (TaskQueue*)malloc(sizeof(TaskQueue));
    q->size = 0;
    q->capacity = init_cap;
    q->data = (Task*)malloc(sizeof(Task) * init_cap);
    return q;
}

void swap(Task *a, Task *b) {
    Task temp = *a;
    *a = *b;
    *b = temp;
}

void push_task(TaskQueue *q, Task t) {
    // Kapasite dolduğunda REALLOC ile genişletme
    if (q->size == q->capacity) {
        q->capacity *= 2;
        q->data = (Task*)realloc(q->data, sizeof(Task) * q->capacity);
        printf("\n[!] Kapasite yetersiz: REALLOC calisti. Yeni Kapasite: %d\n", q->capacity);
    }

    q->data[q->size] = t;
    int i = q->size;
    q->size++;

    // Heapify Up: Yeni eklenen elemanı yukarı taşı
    while (i != 0 && q->data[(i - 1) / 2].urgency > q->data[i].urgency) {
        swap(&q->data[i], &q->data[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Task pop_task(TaskQueue *q) {
    Task root = q->data[0];
    q->data[0] = q->data[q->size - 1];
    q->size--;

    // Heapify Down: Kökü uygun yere indir
    int i = 0;
    while (2 * i + 1 < q->size) {
        int smallest = 2 * i + 1;
        if (2 * i + 2 < q->size && q->data[2 * i + 2].urgency < q->data[smallest].urgency)
            smallest = 2 * i + 2;
        
        if (q->data[i].urgency <= q->data[smallest].urgency) break;
        swap(&q->data[i], &q->data[smallest]);
        i = smallest;
    }
    return root;
}

void free_queue(TaskQueue *q) {
    if (q) {
        free(q->data);
        free(q);
    }
}
