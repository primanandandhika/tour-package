#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PESANAN 50
#define UKURAN_STRUK 60000
#define UKURAN_INPUT 256

typedef struct {
    int kode;
    char nama[40];
    char destinasi[80];
    char durasi[25];
    int jumlahMalam;
    long long harga;
} Paket;

typedef struct {
    Paket paket;
    int jumlahPeserta;
    char transportasi[20];
    long long biayaTransportasi;
    long long biayaPenginapan;
    long long biayaKonsumsi;
    long long biayaGuide;
    long long biayaDokumentasi;
    long long subtotal;
} DetailPesanan;

static char bufferStruk[UKURAN_STRUK];
static size_t posisiStruk = 0;

static void tambahTeks(const char *format, ...) {
    va_list argumenBuffer;
    va_list argumenLayar;
    va_start(argumenBuffer, format);
    va_copy(argumenLayar, argumenBuffer);

    size_t sisa = sizeof(bufferStruk) - posisiStruk;
    int panjang = vsnprintf(bufferStruk + posisiStruk, sisa, format, argumenBuffer);
    va_end(argumenBuffer);

    if (panjang > 0) {
        if ((size_t)panjang >= sisa) {
            posisiStruk = sizeof(bufferStruk) - 1;
        } else {
            posisiStruk += (size_t)panjang;
        }
    }

    vprintf(format, argumenLayar);
    va_end(argumenLayar);
}

static void formatRupiah(long long nilai, char hasil[64]) {
    char angka[32];
    snprintf(angka, sizeof(angka), "%lld", nilai);

    size_t panjang = strlen(angka);
    size_t posisi = 0;
    for (size_t i = 0; i < panjang; ++i) {
        hasil[posisi++] = angka[i];
        size_t sisaDigit = panjang - i - 1;
        if (sisaDigit > 0 && sisaDigit % 3 == 0) {
            hasil[posisi++] = '.';
        }
    }
    hasil[posisi] = '\0';
    strncat(hasil, ",00", 63 - strlen(hasil));
}

static void cetakRupiah(long long nilai) {
    char hasil[64];
    formatRupiah(nilai, hasil);
    printf("%s", hasil);
}

static void cetakRupiahStruk(long long nilai) {
    char hasil[64];
    formatRupiah(nilai, hasil);
    tambahTeks("%s", hasil);
}

static void bersihkanSpasi(char *teks) {
    char *awal = teks;
    while (isspace((unsigned char)*awal)) {
        ++awal;
    }

    char *akhir = awal + strlen(awal);
    while (akhir > awal && isspace((unsigned char)akhir[-1])) {
        --akhir;
    }
    *akhir = '\0';
    if (awal != teks) {
        memmove(teks, awal, (size_t)(akhir - awal) + 1);
    }
}

static void kosongkanSisaBaris(void) {
    int karakter;
    while ((karakter = getchar()) != '\n' && karakter != EOF) {
    }
}

static void bacaBaris(const char *pesan, char *hasil, size_t kapasitas) {
    while (1) {
        printf("%s", pesan);
        fflush(stdout);

        if (fgets(hasil, (int)kapasitas, stdin) == NULL) {
            printf("\nInput dihentikan. Program selesai.\n");
            exit(EXIT_SUCCESS);
        }

        size_t panjang = strlen(hasil);
        if (panjang > 0 && hasil[panjang - 1] == '\n') {
            hasil[panjang - 1] = '\0';
        } else if (!feof(stdin)) {
            kosongkanSisaBaris();
            printf("Input terlalu panjang. Coba lagi.\n");
            continue;
        }

        bersihkanSpasi(hasil);
        return;
    }
}

static long long bacaAngka(const char *pesan, long long minimum, long long maksimum) {
    char baris[UKURAN_INPUT];
    while (1) {
        bacaBaris(pesan, baris, sizeof(baris));
        char *akhir;
        errno = 0;
        long long nilai = strtoll(baris, &akhir, 10);

        if (baris[0] != '\0' && akhir != baris && *akhir == '\0' &&
            errno != ERANGE && nilai >= minimum && nilai <= maksimum) {
            return nilai;
        }

        printf("Input tidak valid. Masukkan angka dari %lld sampai %lld.\n",
               minimum, maksimum);
    }
}

static void bacaTeksWajib(const char *pesan, char *hasil, size_t kapasitas) {
    while (1) {
        bacaBaris(pesan, hasil, kapasitas);
        if (hasil[0] != '\0') {
            return;
        }
        printf("Input tidak boleh kosong.\n");
    }
}

static int namaValid(const char *nama) {
    int adaHuruf = 0;
    for (size_t i = 0; nama[i] != '\0'; ++i) {
        unsigned char karakter = (unsigned char)nama[i];
        if (isalpha(karakter) || karakter >= 128) {
            adaHuruf = 1;
        } else if (!isspace(karakter) && karakter != '-' &&
                   karakter != '\'' && karakter != '.') {
            return 0;
        }
    }
    return adaHuruf;
}

static void bacaNama(const char *pesan, char *hasil, size_t kapasitas) {
    while (1) {
        bacaTeksWajib(pesan, hasil, kapasitas);
        if (namaValid(hasil)) {
            return;
        }
        printf("Nama hanya boleh berisi huruf, spasi, titik, apostrof, atau tanda hubung.\n");
    }
}

static int emailGmailValid(const char *email) {
    const char *at = strchr(email, '@');
    if (at == NULL || at == email || strchr(at + 1, '@') != NULL) {
        return 0;
    }

    const char *domain = at + 1;
    const char *domainWajib = "gmail.com";
    if (strlen(domain) != strlen(domainWajib)) {
        return 0;
    }
    for (size_t i = 0; i < strlen(domainWajib); ++i) {
        if (tolower((unsigned char)domain[i]) != domainWajib[i]) {
            return 0;
        }
    }

    if (email[0] == '.' || at[-1] == '.') {
        return 0;
    }
    int adaKarakterNama = 0;
    for (const char *karakter = email; karakter < at; ++karakter) {
        unsigned char c = (unsigned char)*karakter;
        if (isalnum(c)) {
            adaKarakterNama = 1;
        } else if (c == '.' || c == '_' || c == '+' || c == '-') {
            if (c == '.' && karakter > email && karakter[-1] == '.') {
                return 0;
            }
        } else {
            return 0;
        }
    }
    return adaKarakterNama;
}

static void bacaEmail(char *email, size_t kapasitas) {
    while (1) {
        bacaTeksWajib("Email Gmail: ", email, kapasitas);
        if (emailGmailValid(email)) {
            return;
        }
        printf("Format email tidak valid. Gunakan alamat yang berakhir dengan @gmail.com.\n");
    }
}

static int nomorHPValid(const char *nomor) {
    char digit[20];
    size_t jumlahDigit = 0;
    int adaPlus = 0;

    for (size_t i = 0; nomor[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)nomor[i];
        if (isdigit(c)) {
            if (jumlahDigit >= sizeof(digit) - 1) {
                return 0;
            }
            digit[jumlahDigit++] = (char)c;
        } else if (c == '+' && i == 0) {
            adaPlus = 1;
        } else if (c != ' ' && c != '-' && c != '(' && c != ')') {
            return 0;
        }
    }
    digit[jumlahDigit] = '\0';

    if (adaPlus) {
        return strncmp(digit, "628", 3) == 0 && jumlahDigit >= 11 && jumlahDigit <= 14;
    }
    return strncmp(digit, "08", 2) == 0 && jumlahDigit >= 10 && jumlahDigit <= 13;
}

static void bacaNomorHP(char *nomor, size_t kapasitas) {
    while (1) {
        bacaTeksWajib("No. WhatsApp (08xx atau +62): ", nomor, kapasitas);
        if (nomorHPValid(nomor)) {
            return;
        }
        printf("Nomor HP tidak valid. Contoh: 081234567890 atau +62 812-3456-7890.\n");
    }
}

static int tahunKabisat(int tahun) {
    return tahun % 400 == 0 || (tahun % 4 == 0 && tahun % 100 != 0);
}

static int tanggalValidDanAkanDatang(const char *tanggal) {
    if (strlen(tanggal) != 10 || tanggal[2] != '-' || tanggal[5] != '-') {
        return 0;
    }
    for (int i = 0; i < 10; ++i) {
        if (i != 2 && i != 5 && !isdigit((unsigned char)tanggal[i])) {
            return 0;
        }
    }

    int hari = (tanggal[0] - '0') * 10 + tanggal[1] - '0';
    int bulan = (tanggal[3] - '0') * 10 + tanggal[4] - '0';
    int tahun = (tanggal[6] - '0') * 1000 + (tanggal[7] - '0') * 100 +
                (tanggal[8] - '0') * 10 + tanggal[9] - '0';
    if (tahun < 1900 || bulan < 1 || bulan > 12) {
        return 0;
    }

    const int hariPerBulan[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int batasHari = hariPerBulan[bulan - 1];
    if (bulan == 2 && tahunKabisat(tahun)) {
        batasHari = 29;
    }
    if (hari < 1 || hari > batasHari) {
        return 0;
    }

    time_t sekarang = time(NULL);
    struct tm *waktuLokal = localtime(&sekarang);
    if (waktuLokal != NULL) {
        int tanggalTrip = tahun * 10000 + bulan * 100 + hari;
        int tanggalHariIni = (waktuLokal->tm_year + 1900) * 10000 +
                             (waktuLokal->tm_mon + 1) * 100 + waktuLokal->tm_mday;
        if (tanggalTrip < tanggalHariIni) {
            return 0;
        }
    }
    return 1;
}

static void bacaTanggalTrip(char *tanggal, size_t kapasitas) {
    while (1) {
        bacaTeksWajib("Tanggal Trip (DD-MM-YYYY): ", tanggal, kapasitas);
        if (tanggalValidDanAkanDatang(tanggal)) {
            return;
        }
        printf("Tanggal tidak valid atau sudah lewat. Gunakan format DD-MM-YYYY.\n");
    }
}

static int bacaYaTidak(const char *pesan) {
    char jawaban[20];
    while (1) {
        bacaTeksWajib(pesan, jawaban, sizeof(jawaban));
        if (jawaban[1] == '\0' && (jawaban[0] == 'y' || jawaban[0] == 'Y')) {
            return 1;
        }
        if (jawaban[1] == '\0' && (jawaban[0] == 'n' || jawaban[0] == 'N')) {
            return 0;
        }
        printf("Masukkan y untuk ya atau n untuk tidak.\n");
    }
}

static void cetakTengah(const char *teks, int lebar) {
    int panjang = (int)strlen(teks);
    int padding = panjang < lebar ? (lebar - panjang) / 2 : 0;
    tambahTeks("%*s%s\n", padding, "", teks);
}

static int bacaNomorStruk(void) {
    FILE *file = fopen("counter.txt", "r");
    if (file == NULL) {
        return 1;
    }

    int nomor = 1;
    if (fscanf(file, "%d", &nomor) != 1 || nomor < 1) {
        nomor = 1;
    }
    fclose(file);
    return nomor;
}

static void tulisHeaderDanRincian(const DetailPesanan keranjang[], int jumlahItem,
                                  const char *namaPemesan, const char *nomorHP,
                                  const char *email, const char *alamat,
                                  const char *tanggalTrip, int nomorStruk,
                                  const char *tanggalOrder, const char *jamOrder,
                                  const char *refTransaksi,
                                  int totalPeserta, long long subtotal,
                                  long long diskon, long long ppn,
                                  long long totalBayar) {
    tambahTeks("\n============================================================\n");
    cetakTengah("T R I P F L O W", 60);
    cetakTengah("Your Seamless Escape Starts Here", 60);
    cetakTengah("PT Alur Perjalanan Nusantara | @tripflow.id", 60);
    tambahTeks("============================================================\n");
    tambahTeks("BUKTI PEMBAYARAN & E-VOUCHER #%03d\n", nomorStruk);
    tambahTeks("No. Booking      : TF-%s-%04d\n", tanggalOrder, nomorStruk);
    tambahTeks("RefID Transaksi  : %s\n", refTransaksi);
    tambahTeks("Tanggal Order    : %s %s\n", tanggalOrder, jamOrder);
    tambahTeks("Tanggal Trip     : %s\n", tanggalTrip);
    tambahTeks("------------------------------------------------------------\n");
    tambahTeks("Nama Pemesan     : %s\n", namaPemesan);
    tambahTeks("No. WhatsApp     : %s\n", nomorHP);
    tambahTeks("Email            : %s\n", email);
    tambahTeks("Alamat           : %s\n", alamat);
    tambahTeks("============================================================\n");
    tambahTeks("%-24s %-15s %-6s %-12s\n", "Paket / Add-On", "Harga", "Pax", "Subtotal");
    tambahTeks("------------------------------------------------------------\n");

    for (int i = 0; i < jumlahItem; ++i) {
        const DetailPesanan *detail = &keranjang[i];
        long long biayaPaket = detail->paket.harga * detail->jumlahPeserta;
        tambahTeks("%d. %-21s ", i + 1, detail->paket.nama);
        cetakRupiahStruk(detail->paket.harga);
        tambahTeks(" x %-3d ", detail->jumlahPeserta);
        cetakRupiahStruk(biayaPaket);
        tambahTeks("\n   (%s | %s)\n", detail->paket.durasi, detail->paket.destinasi);

        if (detail->biayaTransportasi > 0) {
            tambahTeks("   + Transport (%s): ", detail->transportasi);
            cetakRupiahStruk(detail->biayaTransportasi);
            tambahTeks("\n");
        }
        if (detail->biayaPenginapan > 0) {
            tambahTeks("   + Penginapan (%d malam): ", detail->paket.jumlahMalam);
            cetakRupiahStruk(detail->biayaPenginapan);
            tambahTeks("\n");
        }
        if (detail->biayaKonsumsi > 0) {
            tambahTeks("   + Konsumsi: ");
            cetakRupiahStruk(detail->biayaKonsumsi);
            tambahTeks("\n");
        }
        if (detail->biayaGuide > 0) {
            tambahTeks("   + Tour Guide: ");
            cetakRupiahStruk(detail->biayaGuide);
            tambahTeks("\n");
        }
        if (detail->biayaDokumentasi > 0) {
            tambahTeks("   + Dokumentasi: ");
            cetakRupiahStruk(detail->biayaDokumentasi);
            tambahTeks("\n");
        }
    }

    tambahTeks("============================================================\n");
    tambahTeks("%-24s : %d pax\n", "Total Peserta", totalPeserta);
    tambahTeks("%-24s : ", "Subtotal Paket & Add-On");
    cetakRupiahStruk(subtotal);
    tambahTeks("\n%-24s : ", "Diskon Promo (5%)");
    cetakRupiahStruk(diskon);
    tambahTeks("\n%-24s : ", "PPN (10%)");
    cetakRupiahStruk(ppn);
    tambahTeks("\n%-24s : ", "TOTAL PEMBAYARAN");
    cetakRupiahStruk(totalBayar);
    tambahTeks("\n============================================================\n");
}

int main(void) {
    const Paket katalog[3] = {
        {1, "Jogja Explorer", "Borobudur, Prambanan, Malioboro", "1 Hari", 0, 250000},
        {2, "Bali Holiday", "Tanah Lot, Pantai Kuta, Uluwatu", "2 Hari 1 Malam", 1, 900000},
        {3, "Labuan Bajo Adventure", "Pulau Padar, Pink Beach, Komodo", "3 Hari 2 Malam", 2, 2500000}
    };

    const long long BIAYA_TRAVEL = 100000;
    const long long BIAYA_BUS = 150000;
    const long long BIAYA_PENGINAPAN = 200000;
    const long long BIAYA_KONSUMSI = 50000;
    const long long BIAYA_GUIDE = 150000;
    const long long BIAYA_DOKUMENTASI = 100000;

    srand((unsigned int)time(NULL));
    int lanjutTransaksi = 1;

    while (lanjutTransaksi) {
        DetailPesanan keranjang[MAX_PESANAN];
        memset(keranjang, 0, sizeof(keranjang));
        int jumlahItem = 0;
        char namaPemesan[60];
        char nomorHP[32];
        char email[100];
        char alamat[120];
        char tanggalTrip[20];

        posisiStruk = 0;
        bufferStruk[0] = '\0';

        printf("\n============================================================\n");
        printf("                   SELAMAT DATANG DI\n");
        printf("                      T R I P F L O W\n");
        printf("             PT Alur Perjalanan Nusantara\n");
        printf("============================================================\n");
        printf("\n--- DATA PEMESAN ---\n");

        bacaNama("Nama lengkap: ", namaPemesan, sizeof(namaPemesan));
        bacaNomorHP(nomorHP, sizeof(nomorHP));
        bacaEmail(email, sizeof(email));
        bacaTeksWajib("Alamat: ", alamat, sizeof(alamat));
        bacaTanggalTrip(tanggalTrip, sizeof(tanggalTrip));

        printf("\n=== KATALOG PAKET WISATA TRIPFLOW ===\n");
        for (int i = 0; i < 3; ++i) {
            printf("%d. %-22s (%s) - Rp. ", katalog[i].kode, katalog[i].nama, katalog[i].durasi);
            cetakRupiah(katalog[i].harga);
            printf("/pax\n   Destinasi: %s\n", katalog[i].destinasi);
        }
        printf("======================================\n");

        while (1) {
            int kode = (int)bacaAngka("\nMasukkan kode paket (0 untuk selesai memilih): ", 0, 3);
            if (kode == 0) {
                if (jumlahItem == 0) {
                    printf("Minimal pilih satu paket wisata.\n");
                    continue;
                }
                break;
            }
            if (jumlahItem >= MAX_PESANAN) {
                printf("Keranjang sudah mencapai batas %d paket.\n", MAX_PESANAN);
                break;
            }

            DetailPesanan detail = {0};
            detail.paket = katalog[kode - 1];
            detail.jumlahPeserta = (int)bacaAngka("Jumlah peserta (1-100): ", 1, 100);

            int transportasi = (int)bacaAngka(
                "Transportasi [1. Travel | 2. Bus | 3. Kendaraan sendiri]: ", 1, 3);
            if (transportasi == 1) {
                strcpy(detail.transportasi, "Travel");
                detail.biayaTransportasi = BIAYA_TRAVEL * detail.jumlahPeserta;
            } else if (transportasi == 2) {
                strcpy(detail.transportasi, "Bus");
                detail.biayaTransportasi = BIAYA_BUS * detail.jumlahPeserta;
            } else {
                strcpy(detail.transportasi, "Kendaraan sendiri");
            }

            if (detail.paket.jumlahMalam > 0 &&
                bacaYaTidak("Tambah penginapan 200rb/pax/malam? (y/n): ")) {
                detail.biayaPenginapan = BIAYA_PENGINAPAN * detail.jumlahPeserta *
                                         detail.paket.jumlahMalam;
            }
            if (bacaYaTidak("Tambah konsumsi 50rb/pax? (y/n): ")) {
                detail.biayaKonsumsi = BIAYA_KONSUMSI * detail.jumlahPeserta;
            }
            if (bacaYaTidak("Tambah tour guide 150rb/grup? (y/n): ")) {
                detail.biayaGuide = BIAYA_GUIDE;
            }
            if (bacaYaTidak("Tambah dokumentasi 100rb/paket? (y/n): ")) {
                detail.biayaDokumentasi = BIAYA_DOKUMENTASI;
            }

            long long biayaPaket = detail.paket.harga * detail.jumlahPeserta;
            detail.subtotal = biayaPaket + detail.biayaTransportasi +
                              detail.biayaPenginapan + detail.biayaKonsumsi +
                              detail.biayaGuide + detail.biayaDokumentasi;
            keranjang[jumlahItem++] = detail;

            printf("Paket '%s' ditambahkan. Subtotal: Rp. ", detail.paket.nama);
            cetakRupiah(detail.subtotal);
            printf("\n");
        }

        int totalPeserta = 0;
        long long subtotalKeseluruhan = 0;
        for (int i = 0; i < jumlahItem; ++i) {
            totalPeserta += keranjang[i].jumlahPeserta;
            subtotalKeseluruhan += keranjang[i].subtotal;
        }

        long long diskon = subtotalKeseluruhan >= 3000000 ? subtotalKeseluruhan * 5 / 100 : 0;
        long long dasarPajak = subtotalKeseluruhan - diskon;
        long long ppn = dasarPajak * 10 / 100;
        long long totalPlusPPN = dasarPajak + ppn;

        time_t sekarang = time(NULL);
        struct tm *waktuPointer = localtime(&sekarang);
        if (waktuPointer == NULL) {
            printf("Gagal membaca waktu sistem.\n");
            return EXIT_FAILURE;
        }
        struct tm waktu = *waktuPointer;
        char bufferDate[16];
        char jamOrder[16];
        if (strftime(bufferDate, sizeof(bufferDate), "%Y%m%d", &waktu) == 0 ||
            strftime(jamOrder, sizeof(jamOrder), "%H:%M:%S", &waktu) == 0) {
            printf("Gagal memformat waktu sistem.\n");
            return EXIT_FAILURE;
        }

        int nomorStruk = bacaNomorStruk();
        int kodeUnik = rand() % 1000;
        char refTransaksi[64];
        snprintf(refTransaksi, sizeof(refTransaksi), "TRX%s-%03d", bufferDate, kodeUnik);

        tulisHeaderDanRincian(keranjang, jumlahItem, namaPemesan, nomorHP, email,
                              alamat, tanggalTrip, nomorStruk, bufferDate, jamOrder,
                              refTransaksi, totalPeserta, subtotalKeseluruhan,
                              diskon, ppn, totalPlusPPN);

        int metode = (int)bacaAngka(
            "\nMetode bayar [1. Tunai | 2. QRIS | 3. E-Wallet | 4. Transfer Bank]: ", 1, 4);
        long long uangBayar = 0;
        char detailMetode[64];
        char refPembayaran[64] = "";

        if (metode == 1) {
            while (1) {
                uangBayar = bacaAngka("Masukkan uang bayar (Rp): ", 0, LLONG_MAX);
                if (uangBayar >= totalPlusPPN) {
                    break;
                }
                printf("Uang kurang. Total yang harus dibayar: Rp. ");
                cetakRupiah(totalPlusPPN);
                printf("\n");
            }
            strcpy(detailMetode, "Tunai");
            tambahTeks("%-24s : %s\n", "Metode Pembayaran", detailMetode);
            tambahTeks("%-24s : ", "Uang Bayar Masuk");
            cetakRupiahStruk(uangBayar);
            tambahTeks("\n%-24s : ", "Kembalian");
            cetakRupiahStruk(uangBayar - totalPlusPPN);
            tambahTeks("\n%-24s : %s\n", "Status", "LUNAS");
        } else if (metode == 2) {
            snprintf(refPembayaran, sizeof(refPembayaran), "QRS%s-%03d-%03d",
                     bufferDate, nomorStruk, kodeUnik);
            strcpy(detailMetode, "QRIS (simulasi)");
            tambahTeks("%-24s : %s\n", "Metode Pembayaran", detailMetode);
            tambahTeks("%-24s : %s\n", "RefID QRIS", refPembayaran);
            tambahTeks("%-24s : %s\n", "Status", "LUNAS (SIMULASI)");
        } else if (metode == 3) {
            const char *daftarEwallet[] = {"OVO", "GoPay", "DANA", "ShopeePay"};
            int pilihan = (int)bacaAngka("Pilih E-Wallet [1. OVO | 2. GoPay | 3. DANA | 4. ShopeePay]: ", 1, 4);
            snprintf(refPembayaran, sizeof(refPembayaran), "EWL%s-%03d-%03d",
                     bufferDate, nomorStruk, kodeUnik);
            snprintf(detailMetode, sizeof(detailMetode), "%s (simulasi)", daftarEwallet[pilihan - 1]);
            tambahTeks("%-24s : E-Wallet (%s)\n", "Metode Pembayaran", daftarEwallet[pilihan - 1]);
            tambahTeks("%-24s : %s\n", "RefID E-Wallet", refPembayaran);
            tambahTeks("%-24s : %s\n", "Status", "LUNAS (SIMULASI)");
        } else {
            const char *daftarBank[] = {"BCA", "Mandiri", "BNI", "BRI"};
            int pilihan = (int)bacaAngka("Pilih Bank [1. BCA | 2. Mandiri | 3. BNI | 4. BRI]: ", 1, 4);
            snprintf(refPembayaran, sizeof(refPembayaran), "VA%s-%03d-%03d",
                     bufferDate, nomorStruk, kodeUnik);
            snprintf(detailMetode, sizeof(detailMetode), "%s Virtual Account (simulasi)", daftarBank[pilihan - 1]);
            tambahTeks("%-24s : %s Virtual Account\n", "Metode Pembayaran", daftarBank[pilihan - 1]);
            tambahTeks("%-24s : %s\n", "Nomor Virtual Account", refPembayaran);
            tambahTeks("%-24s : %s\n", "Status", "LUNAS (SIMULASI)");
        }
        tambahTeks("============================================================\n");

        tambahTeks("CATATAN PENTING:\n");
        tambahTeks("1. Tunjukkan e-voucher ini ke Tour Leader di titik kumpul.\n");
        tambahTeks("2. Grup WhatsApp Trip dikirim pada H-3.\n");
        tambahTeks("3. Bawa KTP/KTM asli saat registrasi ulang.\n");
        tambahTeks("------------------------------------------------------------\n");
        cetakTengah("Terima kasih sudah healing bareng TripFlow!", 60);
        cetakTengah("Chat MinFlow (24/7): wa.me/6281100001234", 60);
        tambahTeks("Diskon 5%% otomatis untuk transaksi >= Rp 3.000.000\n");
        tambahTeks("============================================================\n");

        char namaFile[64];
        snprintf(namaFile, sizeof(namaFile), "TRIPFLOW_STRUK%03d.txt", nomorStruk);
        FILE *fileStruk = fopen(namaFile, "w");
        if (fileStruk == NULL) {
            printf("Gagal membuat file struk '%s'.\n", namaFile);
        } else {
            int strukBerhasilDitulis = fputs(bufferStruk, fileStruk) >= 0;
            if (fclose(fileStruk) != 0) {
                strukBerhasilDitulis = 0;
            }

            if (strukBerhasilDitulis) {
                printf("\nE-voucher dan struk tersimpan di '%s'.\n", namaFile);
                FILE *fileCounter = fopen("counter.txt", "w");
                if (fileCounter != NULL) {
                    fprintf(fileCounter, "%d\n", nomorStruk + 1);
                    fclose(fileCounter);
                } else {
                    printf("Peringatan: nomor struk berikutnya gagal disimpan.\n");
                }
            } else {
                printf("Gagal menyimpan isi struk.\n");
            }
        }

        lanjutTransaksi = bacaYaTidak("\nApakah ingin melakukan pemesanan lain? (y/n): ");
    }

    printf("\nSistem pemesanan TripFlow selesai. Terima kasih!\n");
    return 0;
}
