#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- 1. YAPI TANIMLARI (Normalde .h olan kısımlar) ---

typedef struct {
    int gorev_id;
    char servis_adi[100];
    int oncelik;       // u= parametresi (0-7 arası)
    bool parcali_mi;   // i= parametresi (?1 veya ?0)
} Gorev;

typedef struct {
    Gorev *dizi;
    int kapasite;
    int boyut;
} GorevHeap;

// --- 2. AYRIŞTIRICI FONKSİYON (Normalde rfc_parser.c olan kısım) ---

void parametre_ayristir(const char *header, Gorev *g) {
    // Varsayılan değerler
    g->oncelik = 3; 
    g->parcali_mi = false;

    if (header == NULL) return;

    // "u=" (Urgency) tespiti
    const char *u_konum = strstr(header, "u=");
    if (u_konum != NULL) {
        sscanf(u_konum, "u=%d", &g->oncelik);
        if (g->oncelik < 0 || g->oncelik > 7) g->oncelik = 3;
    }

    // "i=" (Incremental) tespiti
    const char *i_konum = strstr(header, "i=");
    if (i_konum != NULL) {
        if (strstr(i_konum, "?1") != NULL) g->parcali_mi = true;
    }
}

// --- 3. HEAP FONKSİYONLARI (Normalde min_heap.c olan kısımlar) ---

GorevHeap* heap_baslat(int ilk_kapasite) {
    GorevHeap *h = (GorevHeap *)malloc(sizeof(GorevHeap));
    h->kapasite = ilk_kapasite;
    h->boyut = 0;
    h->dizi = (Gorev *)malloc(ilk_kapasite * sizeof(Gorev));
    return h;
}

void yer_degistir(Gorev *a, Gorev *b) {
    Gorev gecici = *a;
    *a = *b;
    *b = gecici;
}

void yukari_dengele(GorevHeap *h, int index) {
    int ebeveyn = (index - 1) / 2;
    if (index > 0 && h->dizi[index].oncelik < h->dizi[ebeveyn].oncelik) {
        yer_degistir(&h->dizi[index], &h->dizi[ebeveyn]);
        yukari_dengele(h, ebeveyn);
    }
}

void asagi_dengele(GorevHeap *h, int index) {
    int kucuk = index;
    int sol = 2 * index + 1;
    int sag = 2 * index + 2;

    if (sol < h->boyut && h->dizi[sol].oncelik < h->dizi[kucuk].oncelik) kucuk = sol;
    if (sag < h->boyut && h->dizi[sag].oncelik < h->dizi[kucuk].oncelik) kucuk = sag;

    if (kucuk != index) {
        yer_degistir(&h->dizi[index], &h->dizi[kucuk]);
        asagi_dengele(h, kucuk);
    }
}

void gorev_ekle(GorevHeap *h, Gorev g) {
    // Dinamik Bellek Yönetimi (REALLOC)
    if (h->boyut == h->kapasite) {
        h->kapasite *= 2;
        h->dizi = (Gorev *)realloc(h->dizi, h->kapasite * sizeof(Gorev));
        printf("\n[!] Bellek Yetmedi: Kapasite %d yapildi.\n", h->kapasite);
    }
    h->dizi[h->boyut] = g;
    yukari_dengele(h, h->boyut);
    h->boyut++;
}

Gorev en_oncelikliyi_cek(GorevHeap *h) {
    Gorev kok = h->dizi[0];
    h->dizi[0] = h->dizi[h->boyut - 1];
    h->boyut--;
    asagi_dengele(h, 0);
    return kok;
}

// --- 4. ANA PROGRAM ---

int main() {
    printf("--- SISTEM GOREV ZAMANLAYICI BASLATILDI ---\n\n");

    // Realloc'u görmek için kapasiteyi 2'den başlatıyoruz
    GorevHeap *isleyici = heap_baslat(2);

    struct {
        int id;
        char servis[50];
        char baslik[100];
    } ham_liste[] = {
        {1, "Veritabani", "u=6, i=?1"},
        {2, "Kullanici_Giris", "u=0, i=?0"}, // En acil
        {3, "Odeme_Sistemi", "u=1, i=?0"},   // Çok acil
        {4, "Log_Kaydi", "u=7, i=?1"},
        {5, "Resim_Yukleme", "u=3, i=?1"}
    };

    int adet = sizeof(ham_liste) / sizeof(ham_liste[0]);

    for (int i = 0; i < adet; i++) {
        Gorev g;
        g.gorev_id = ham_liste[i].id;
        strcpy(g.servis_adi, ham_liste[i].servis);
        
        // Parametreleri ayıkla
        parametre_ayristir(ham_liste[i].baslik, &g);
        
        // Kuyruğa yerleştir
        gorev_ekle(isleyici, g);
        printf("[+] Eklendi: %-15s | Oncelik: %d\n", g.servis_adi, g.oncelik);
    }

    printf("\n--- ISLEME SIRASI (EN ACIL -> EN DUSUK) ---\n");
    printf("-------------------------------------------\n");

    while (isleyici->boyut > 0) {
        Gorev g = en_oncelikliyi_cek(isleyici);
        printf("CALISIYOR -> ID: %d | Servis: %-15s | Oncelik: %d | Parcali: %s\n", 
               g.gorev_id, g.servis_adi, g.oncelik, g.parcali_mi ? "EVET" : "HAYIR");
    }

    // Temizlik
    free(isleyici->dizi);
    free(isleyici);
    printf("\n[+] Sistem temizlendi, cikiliyor.\n");

    return 0;
}
