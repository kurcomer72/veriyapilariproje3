#ifndef TASK_PARSER_H
#define TASK_PARSER_H

#include <stdbool.h>

// Orijinal "Request" yapısının benzeri: Görev detaylarını tutar
typedef struct {
    int task_id;        // İstek ID yerine Görev ID
    int priority;       // Urgency yerine Öncelik (0-7)
    bool is_async;      // Incremental yerine Asenkron durumu
    char service[128];  // Path yerine Servis/Yol adı
} Task;

// Header string'ini alıp Task yapısını dolduran fonksiyon prototipi
// Orijinal "parse_rfc9213" fonksiyonunun benzeri
void process_task_params(const char *metadata, Task *t);

#endif
