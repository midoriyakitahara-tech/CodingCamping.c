/*
 * ============================================================
 *  PROGRAM  : Adventure Camp - Sistem Penyewaan Peralatan
 *  BAHASA   : C
 *  NAMA     : (isi nama lengkap)
 *  NIM      : (isi NIM)
 *
 *  FITUR    : Satu transaksi dapat berisi BANYAK jenis peralatan.
 *             Diskon dihitung dari total seluruh peralatan
 *             berdasarkan lama penyewaan.
 * ============================================================
 */

#include <stdio.h>
#include <string.h>

/* ---------- LANGKAH 1: Konstanta & data global ---------- */
#define JUMLAH_ALAT      5
#define MAKS_TRANSAKSI   10
#define PANJANG_NAMA     50

/* Array data peralatan */
const char *namaAlat[JUMLAH_ALAT] = {
    "Tenda Kapasitas 4 Orang", "Sleeping Bag", "Kompor Portable",
    "Matras Camping", "Lampu Camping"
};
const char *namaSingkat[JUMLAH_ALAT] = {
    "Tenda", "Sleeping Bag", "Kompor", "Matras", "Lampu Camping"
};
const int hargaAlat[JUMLAH_ALAT] = {50000, 15000, 20000, 10000, 12000};

/* Array riwayat transaksi.
 * riwayatQty[t][i] = jumlah unit peralatan ke-i pada transaksi ke-t
 * (bernilai 0 jika peralatan tersebut tidak disewa). */
char riwayatNama[MAKS_TRANSAKSI][PANJANG_NAMA];
int  riwayatQty[MAKS_TRANSAKSI][JUMLAH_ALAT];
int  riwayatLama[MAKS_TRANSAKSI];
int  riwayatTotal[MAKS_TRANSAKSI];
int  jumlahTransaksi = 0;

/* ---------- LANGKAH 2: Fungsi bantu input (validasi) ---------- */

/* Membaca bilangan bulat dalam rentang [min, max]; ulangi jika salah. */
int bacaInteger(const char *pesan, int min, int max) {
    char buffer[100];
    int nilai;
    char sisa;

    while (1) {
        printf("%s", pesan);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return min; /* input berakhir (EOF) */
        }
        if (sscanf(buffer, "%d %c", &nilai, &sisa) == 1 &&
            nilai >= min && nilai <= max) {
            return nilai;
        }
        printf("Input tidak valid! Masukkan angka %d - %d.\n", min, max);
    }
}

/* Membaca nama penyewa; tidak boleh kosong. */
void bacaNama(char *nama) {
    while (1) {
        printf("Nama penyewa          : ");
        if (fgets(nama, PANJANG_NAMA, stdin) == NULL) {
            strcpy(nama, "Tanpa Nama");
            return;
        }
        nama[strcspn(nama, "\n")] = '\0';
        if (strlen(nama) > 0) {
            return;
        }
        printf("Nama tidak boleh kosong!\n");
    }
}

/* Menanyakan Y/T sampai valid. Mengembalikan 1 jika Y, 0 jika T. */
int tanyaYT(const char *pesan) {
    char buffer[100];

    while (1) {
        printf("%s", pesan);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return 0;
        }
        if (buffer[0] == 'Y' || buffer[0] == 'y') return 1;
        if (buffer[0] == 'T' || buffer[0] == 't') return 0;
        printf("Masukkan Y atau T saja!\n");
    }
}

void tungguEnter(void) {
    char buffer[100];
    printf("\nTekan Enter untuk kembali ke menu utama...");
    fgets(buffer, sizeof(buffer), stdin);
}

/* ---------- LANGKAH 3: Fungsi tampilan ---------- */

void tampilkanMenu(void) {
    printf("\n========================================\n");
    printf("            ADVENTURE CAMP\n");
    printf("      SISTEM PENYEWAAN PERALATAN\n");
    printf("========================================\n");
    printf("1. Lihat Daftar Peralatan\n");
    printf("2. Sewa Peralatan\n");
    printf("3. Lihat Riwayat Transaksi\n");
    printf("4. Keluar\n");
    printf("========================================\n");
}

void tampilkanDaftarPeralatan(void) {
    int i;
    printf("\n========================================\n");
    printf("          DAFTAR PERALATAN\n");
    printf("========================================\n");
    printf("%-4s%-26s%s\n", "No", "Peralatan", "Harga/Hari");
    printf("----------------------------------------\n");
    for (i = 0; i < JUMLAH_ALAT; i++) {
        printf("%-4d%-26sRp%d\n", i + 1, namaAlat[i], hargaAlat[i]);
    }
    printf("========================================\n");
}

/* Menampilkan isi keranjang sementara (peralatan yang sudah dipilih). */
void tampilkanKeranjang(const int qty[], int lama) {
    int i, no = 0;
    printf("\n--- Keranjang saat ini ---\n");
    for (i = 0; i < JUMLAH_ALAT; i++) {
        if (qty[i] > 0) {
            no++;
            printf("%d. %-24s x%d\n", no, namaAlat[i], qty[i]);
        }
    }
    printf("Lama sewa: %d hari\n", lama);
    printf("--------------------------\n");
}

/* ---------- LANGKAH 4: Perhitungan ---------- */

/* Diskon berdasarkan lama sewa: 1-2 hari 0%, 3-4 hari 10%, >=5 hari 15% */
int hitungDiskonPersen(int lamaHari) {
    if (lamaHari >= 5) return 15;
    if (lamaHari >= 3) return 10;
    return 0;
}

/* Menghitung subtotal seluruh peralatan di keranjang untuk lama sewa tertentu. */
int hitungSubtotal(const int qty[], int lama) {
    int i, subtotal = 0;
    for (i = 0; i < JUMLAH_ALAT; i++) {
        subtotal += hargaAlat[i] * qty[i] * lama;
    }
    return subtotal;
}

void tampilkanStruk(const char *nama, const int qty[], int lama,
                    int subtotal, int persen, int diskon, int total) {
    int i, no = 0;
    char labelDiskon[20];

    printf("\n========================================\n");
    printf("            ADVENTURE CAMP\n");
    printf("           STRUK PENYEWAAN\n");
    printf("========================================\n");
    printf("Nama Penyewa   : %s\n", nama);
    printf("Lama Sewa      : %d hari\n", lama);
    printf("----------------------------------------\n");
    printf("Rincian Peralatan:\n");
    for (i = 0; i < JUMLAH_ALAT; i++) {
        if (qty[i] > 0) {
            no++;
            printf("%d. %s\n", no, namaAlat[i]);
            printf("   %d unit x Rp%d x %d hari = Rp%d\n",
                   qty[i], hargaAlat[i], lama, hargaAlat[i] * qty[i] * lama);
        }
    }
    printf("----------------------------------------\n");
    snprintf(labelDiskon, sizeof(labelDiskon), "Diskon (%d%%)", persen);
    printf("%-15s: Rp%d\n", "Subtotal", subtotal);
    printf("%-15s: Rp%d\n", labelDiskon, diskon);
    printf("----------------------------------------\n");
    printf("%-15s: Rp%d\n", "TOTAL BAYAR", total);
    printf("========================================\n");
    printf("Terima kasih telah menggunakan\n");
    printf("layanan Adventure Camp!\n");
}

/* ---------- LANGKAH 5: Proses penyewaan (banyak barang) ---------- */

/* Mengembalikan 1 jika transaksi berhasil diproses, 0 jika gagal (penuh). */
int prosesSewa(void) {
    char nama[PANJANG_NAMA];
    int qty[JUMLAH_ALAT] = {0};   /* keranjang: jumlah unit per peralatan */
    int pilihan, jumlah, lama, i;
    int subtotal, persen, diskon, total;
    int tambahLagi;

    if (jumlahTransaksi >= MAKS_TRANSAKSI) {
        printf("\nKapasitas transaksi penuh (maks %d)! Transaksi baru tidak dapat diterima.\n",
               MAKS_TRANSAKSI);
        return 0;
    }

    printf("\n========================================\n");
    printf("            SEWA PERALATAN\n");
    printf("========================================\n");

    bacaNama(nama);
    lama = bacaInteger("Lama sewa/hari (min 1)      : ", 1, 365);
    tampilkanDaftarPeralatan();

    /* Perulangan: tambah peralatan sampai pengguna selesai */
    do {
        pilihan = bacaInteger("Pilih nomor peralatan [1-5]: ", 1, JUMLAH_ALAT);
        jumlah  = bacaInteger("Jumlah unit (min 1)         : ", 1, 100);

        /* Jika peralatan yang sama dipilih lagi, jumlahnya ditambahkan */
        qty[pilihan - 1] += jumlah;
        printf("Ditambahkan: %s x%d\n", namaAlat[pilihan - 1], jumlah);

        tampilkanKeranjang(qty, lama);
        tambahLagi = tanyaYT("Tambah peralatan lain? (Y/T): ");
    } while (tambahLagi);

    /* Perhitungan biaya seluruh peralatan */
    subtotal = hitungSubtotal(qty, lama);
    persen   = hitungDiskonPersen(lama);
    diskon   = subtotal * persen / 100;
    total    = subtotal - diskon;

    /* Simpan ke riwayat */
    strcpy(riwayatNama[jumlahTransaksi], nama);
    for (i = 0; i < JUMLAH_ALAT; i++) {
        riwayatQty[jumlahTransaksi][i] = qty[i];
    }
    riwayatLama[jumlahTransaksi]  = lama;
    riwayatTotal[jumlahTransaksi] = total;
    jumlahTransaksi++;

    tampilkanStruk(nama, qty, lama, subtotal, persen, diskon, total);
    return 1;
}

/* ---------- LANGKAH 6: Riwayat transaksi ---------- */

/* Menyusun teks ringkas peralatan pada transaksi ke-t ke dalam 'hasil'. */
void ringkasPeralatan(int t, char *hasil) {
    int i, jenis = 0, idx = 0;
    for (i = 0; i < JUMLAH_ALAT; i++) {
        if (riwayatQty[t][i] > 0) {
            jenis++;
            idx = i;
        }
    }
    if (jenis == 1) {
        strcpy(hasil, namaSingkat[idx]);
    } else {
        sprintf(hasil, "%d jenis alat", jenis);
    }
}

void tampilkanRiwayat(void) {
    int i, j, totalPendapatan = 0;
    char ringkas[30];

    printf("\n========================================\n");
    printf("          RIWAYAT TRANSAKSI\n");
    printf("========================================\n");

    if (jumlahTransaksi == 0) {
        printf("Belum ada transaksi penyewaan.\n");
        return;
    }

    printf("%-4s%-10s%-16s%s\n", "No", "Nama", "Peralatan", "Total");
    printf("----------------------------------------\n");
    for (i = 0; i < jumlahTransaksi; i++) {
        ringkasPeralatan(i, ringkas);
        printf("%-4d%-10s%-16sRp%d\n", i + 1, riwayatNama[i], ringkas, riwayatTotal[i]);
        /* Rincian barang pada transaksi ini */
        for (j = 0; j < JUMLAH_ALAT; j++) {
            if (riwayatQty[i][j] > 0) {
                printf("      - %s x%d (%d hari)\n",
                       namaSingkat[j], riwayatQty[i][j], riwayatLama[i]);
            }
        }
        totalPendapatan += riwayatTotal[i];
    }
    printf("----------------------------------------\n");
    printf("Jumlah Transaksi : %d\n", jumlahTransaksi);
    printf("Total Pendapatan : Rp%d\n", totalPendapatan);
    printf("========================================\n");
}

/* ---------- LANGKAH 7: Program utama (menu & perulangan) ---------- */

int main(void) {
    int pilihan;
    int lanjut = 1;

    while (lanjut) {
        tampilkanMenu();
        pilihan = bacaInteger("Pilih menu [1-4]: ", 1, 4);

        switch (pilihan) {
            case 1:
                tampilkanDaftarPeralatan();
                tungguEnter();
                break;
            case 2:
                prosesSewa();
                lanjut = tanyaYT("\nKembali ke menu utama? (Y/T): ");
                break;
            case 3:
                tampilkanRiwayat();
                tungguEnter();
                break;
            case 4:
                lanjut = 0;
                break;
        }
    }

    printf("\nTerima kasih telah menggunakan Adventure Camp. Sampai jumpa!\n");
    return 0;
}
