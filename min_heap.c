#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// --- Veri Yapıları ---
typedef struct {
    int patient_id;
    char name[50];
    int acuity_level; // 1: Kritik, 5: Rutin Kontrol
    char complaint[100];
} Patient;

typedef struct {
    Patient *patients;
    int current_count;
    int max_capacity;
} TriageQueue;

// --- Fonksiyonlar ---

// Yeni bir triyaj kuyruğu oluşturur
TriageQueue* init_triage(int initial_cap) {
    TriageQueue *triage = (TriageQueue *)malloc(sizeof(TriageQueue));
    if (!triage) return NULL;

    triage->max_capacity = initial_cap;
    triage->current_count = 0;
    triage->patients = (Patient *)malloc(initial_cap * sizeof(Patient));
    
    return triage;
}

// İki hastanın yerini değiştirir
void swap_patients(Patient *p1, Patient *p2) {
    Patient temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

// Yeni hasta eklendiğinde yukarı doğru sıralama (Bubble Up)
void sift_up(TriageQueue *tq, int idx) {
    while (idx > 0) {
        int parent_idx = (idx - 1) / 2;
        // Acuity Level düşük olan daha önceliklidir
        if (tq->patients[idx].acuity_level < tq->patients[parent_idx].acuity_level) {
            swap_patients(&tq->patients[idx], &tq->patients[parent_idx]);
            idx = parent_idx;
        } else {
            break;
        }
    }
}

// Hasta taburcu edildiğinde aşağı doğru sıralama (Sift Down)
void sift_down(TriageQueue *tq, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < tq->current_count && 
        tq->patients[left].acuity_level < tq->patients[smallest].acuity_level) {
        smallest = left;
    }

    if (right < tq->current_count && 
        tq->patients[right].acuity_level < tq->patients[smallest].acuity_level) {
        smallest = right;
    }

    if (smallest != idx) {
        swap_patients(&tq->patients[idx], &tq->patients[smallest]);
        sift_down(tq, smallest);
    }
}

// Kuyruğa yeni hasta girişi yap
void register_patient(TriageQueue *tq, Patient p) {
    // Kapasite kontrolü ve Dinamik Bellek Artırımı
    if (tq->current_count == tq->max_capacity) {
        tq->max_capacity *= 2;
        tq->patients = (Patient *)realloc(tq->patients, tq->max_capacity * sizeof(Patient));
        printf("\n[!] Kapasite doldu. Yeni triyaj alani acildi: %d\n", tq->max_capacity);
    }

    tq->patients[tq->current_count] = p;
    sift_up(tq, tq->current_count);
    tq->current_count++;
}

// En acil hastayı muayeneye çağır
Patient call_next_patient(TriageQueue *tq) {
    if (tq->current_count <= 0) {
        Patient empty = {-1, "N/A", 999, ""};
        return empty;
    }

    Patient next_up = tq->patients[0];
    
    // Son hastayı başa al ve yeniden düzenle
    tq->patients[0] = tq->patients[tq->current_count - 1];
    tq->current_count--;
    
    if (tq->current_count > 0) {
        sift_down(tq, 0);
    }

    return next_up;
}

void clear_triage(TriageQueue *tq) {
    if (tq) {
        free(tq->patients);
        free(tq);
    }
}

int main() {
    printf("=== ACIL SERVIS HASTA YONETIM SISTEMI ===\n\n");

    TriageQueue *er_queue = init_triage(2);

    Patient list[] = {
        {1, "Ahmet Yilmaz", 3, "Yuksek Ates"},
        {2, "Ayse Kaya", 1, "Kalp Spazmi"},      // En Acil
        {3, "Mehmet Oz", 5, "Recete Yenileme"}, // En Düşük Öncelik
        {4, "Selin Can", 2, "Kesik Yarasi"},
        {5, "Veli Bak", 1, "Solunum Guclugu"}    // Kritik
    };

    for(int i = 0; i < 5; i++) {
        printf("[+] Hasta Kaydi: %-15s (Oncelik: %d)\n", list[i].name, list[i].acuity_level);
        register_patient(er_queue, list[i]);
    }

    printf("\n--- Muayene Sirasi ---\n");
    while (er_queue->current_count > 0) {
        Patient p = call_next_patient(er_queue);
        printf("DOKTOR ODASINA: [ID: %d] %-15s | Seviye: %d | Sikayet: %s\n", 
               p.patient_id, p.name, p.acuity_level, p.complaint);
    }

    clear_triage(er_queue);
    printf("\nTum hastalar muayene edildi. Sistem kapaniyor.\n");

    return 0;
}
