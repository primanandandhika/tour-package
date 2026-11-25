#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Destinasi {
    string jenisWisata;
    string nama;
    long long hargaPerHari;
};

struct PilihanHarga {
    string nama;
    long long harga;
};

struct RincianTambahan {
    long long harga;
    vector<string> nama;
};

const vector<Destinasi> daftarDestinasi = {
    {"Alam", "Gunung Bromo", 350000},
    {"Alam", "Bandung", 250000},
    {"Budaya", "Yogyakarta", 300000},
    {"Budaya", "Solo", 275000},
    {"Bahari", "Bali", 450000},
    {"Bahari", "Lombok", 500000}
};

const vector<PilihanHarga> daftarLayanan = {
    {"Kendaraan pribadi dengan pemandu wisata", 75000},
    {"Transportasi penyelenggara dengan pemandu wisata", 150000}
};

const vector<PilihanHarga> daftarPaket = {
    {"Tanpa paket tambahan", 0},
    {"Paket penginapan", 250000},
    {"Paket penginapan dan makan", 350000}
};

const vector<PilihanHarga> daftarTambahan = {
    {"Asuransi perjalanan", 50000},
    {"Dokumentasi foto", 100000},
    {"Aktivitas tambahan", 150000}
};

string formatRupiah(long long jumlah) {
    string angka = to_string(jumlah);
    string hasil;
    int digitDalamKelompok = 0;

    for (int i = static_cast<int>(angka.size()) - 1; i >= 0; --i) {
        hasil += angka[i];
        ++digitDalamKelompok;

        if (digitDalamKelompok == 3 && i > 0) {
            hasil += '.';
            digitDalamKelompok = 0;
        }
    }

    reverse(hasil.begin(), hasil.end());
    return "Rp " + hasil;
}

long long bacaAngka(const string& pesan, long long minimum, long long maksimum) {
    while (true) {
        cout << pesan;

        string baris;
        getline(cin, baris);
        stringstream input(baris);
        long long angka;
        char karakterTambahan;

        if (input >> angka && !(input >> karakterTambahan) &&
            angka >= minimum && angka <= maksimum) {
            return angka;
        }

        cout << "Masukkan angka antara " << minimum << " dan " << maksimum
             << ".\n";
    }
}

string bacaTeks(const string& pesan) {
    while (true) {
        cout << pesan;

        string teks;
        getline(cin, teks);

        if (!teks.empty()) {
            return teks;
        }

        cout << "Input tidak boleh kosong.\n";
    }
}

bool bacaKonfirmasi(const string& pesan) {
    while (true) {
        string jawaban = bacaTeks(pesan);
        for (char& karakter : jawaban) {
            karakter = static_cast<char>(tolower(static_cast<unsigned char>(karakter)));
        }

        if (jawaban == "y" || jawaban == "ya") {
            return true;
        }
        if (jawaban == "t" || jawaban == "tidak") {
            return false;
        }

        cout << "Masukkan y/ya atau t/tidak.\n";
    }
}

void tampilkanPilihan(const vector<PilihanHarga>& pilihan) {
    for (size_t i = 0; i < pilihan.size(); ++i) {
        cout << i + 1 << ". " << pilihan[i].nama << " - "
             << formatRupiah(pilihan[i].harga) << '\n';
    }
}

int pilihDariMenu(const vector<PilihanHarga>& pilihan, const string& pesan) {
    tampilkanPilihan(pilihan);
    return static_cast<int>(bacaAngka(pesan, 1, pilihan.size())) - 1;
}

Destinasi pilihDestinasi() {
    const vector<string> jenisWisata = {"Alam", "Budaya", "Bahari"};

    cout << "\nDaftar jenis wisata:\n";
    for (size_t i = 0; i < jenisWisata.size(); ++i) {
        cout << i + 1 << ". " << jenisWisata[i] << '\n';
    }

    int pilihanJenis = static_cast<int>(
        bacaAngka("Pilih jenis wisata: ", 1, jenisWisata.size())) - 1;
    string jenisDipilih = jenisWisata[pilihanJenis];
    vector<Destinasi> destinasiTersedia;

    cout << "\nDestinasi wisata " << jenisDipilih << ":\n";
    for (const Destinasi& destinasi : daftarDestinasi) {
        if (destinasi.jenisWisata == jenisDipilih) {
            destinasiTersedia.push_back(destinasi);
            cout << destinasiTersedia.size() << ". " << destinasi.nama << " - "
                 << formatRupiah(destinasi.hargaPerHari)
                 << " per peserta per hari\n";
        }
    }

    int pilihanDestinasi = static_cast<int>(
        bacaAngka("Pilih destinasi: ", 1, destinasiTersedia.size())) - 1;
    return destinasiTersedia[pilihanDestinasi];
}

RincianTambahan pilihTambahan() {
    vector<bool> sudahDipilih(daftarTambahan.size(), false);
    RincianTambahan rincian = {};

    cout << "\nPilih tambahan. Masukkan 0 jika sudah selesai.\n";
    for (size_t i = 0; i < daftarTambahan.size(); ++i) {
        cout << i + 1 << ". " << daftarTambahan[i].nama << " - "
             << formatRupiah(daftarTambahan[i].harga)
             << " per peserta\n";
    }
    cout << "0. Selesai memilih tambahan\n";

    while (true) {
        int pilihan = static_cast<int>(
            bacaAngka("Pilih tambahan: ", 0, daftarTambahan.size()));

        if (pilihan == 0) {
            return rincian;
        }

        size_t indeks = static_cast<size_t>(pilihan - 1);
        if (sudahDipilih[indeks]) {
            cout << "Pilihan itu sudah ditambahkan. Pilih yang lain atau masukkan 0.\n";
            continue;
        }

        sudahDipilih[indeks] = true;
        rincian.harga += daftarTambahan[indeks].harga;
        rincian.nama.push_back(daftarTambahan[indeks].nama);
        cout << daftarTambahan[indeks].nama << " ditambahkan.\n";
    }
}

bool simpanPemesanan(const string& namaPemesan,
                     const vector<string>& namaPeserta,
                     const Destinasi& destinasi,
                     int durasiHari,
                     const PilihanHarga& layanan,
                     const PilihanHarga& paket,
                     const vector<string>& tambahan,
                     long long budget,
                     long long totalBiaya,
                     const string& metodePembayaran) {
    ofstream file("pemesanan_paket_wisata.txt", ios::app);
    if (!file) {
        return false;
    }

    file << "========================================\n";
    file << "PEMESANAN PAKET WISATA\n";
    file << "Nama pemesan: " << namaPemesan << '\n';
    file << "Destinasi: " << destinasi.nama << " (" << destinasi.jenisWisata << ")\n";
    file << "Durasi: " << durasiHari << " hari\n";
    file << "Layanan: " << layanan.nama << '\n';
    file << "Paket: " << paket.nama << '\n';
    file << "Tambahan: ";

    if (tambahan.empty()) {
        file << "Tidak ada\n";
    } else {
        for (size_t i = 0; i < tambahan.size(); ++i) {
            if (i > 0) {
                file << ", ";
            }
            file << tambahan[i];
        }
        file << '\n';
    }

    file << "Jumlah peserta: " << namaPeserta.size() << '\n';
    file << "Daftar peserta:\n";
    for (size_t i = 0; i < namaPeserta.size(); ++i) {
        file << i + 1 << ". " << namaPeserta[i] << '\n';
    }

    file << "Budget: " << formatRupiah(budget) << '\n';
    file << "Total biaya: " << formatRupiah(totalBiaya) << '\n';
    file << "Metode pembayaran: " << metodePembayaran << '\n';
    file << "Status pembayaran: BERHASIL\n";
    file << "\nITINERARY:\n";
    for (int hari = 1; hari <= durasiHari; ++hari) {
        file << "Hari " << hari << ": wisata " << destinasi.jenisWisata
             << " di " << destinasi.nama << " dengan layanan "
             << layanan.nama << ".\n";
    }

    file << "\nDokumen perjalanan: e-voucher, kartu identitas, dan bukti pembayaran.\n";
    file << "Persiapan: periksa jadwal dan barang bawaan, serta ikuti informasi "
         << "keberangkatan dari penyelenggara.\n";
    file << "========================================\n\n";

    return static_cast<bool>(file);
}

int main() {
    cout << "========================================\n";
    cout << "       PROGRAM PEMESANAN PAKET WISATA\n";
    cout << "========================================\n";
    cout << "Harga di katalog adalah contoh harga per peserta.\n";

    Destinasi destinasi = pilihDestinasi();
    int durasiHari = static_cast<int>(
        bacaAngka("Masukkan durasi wisata (1-14 hari): ", 1, 14));

    cout << "\nPilihan layanan (pemandu wisata selalu termasuk; harga per peserta per hari):\n";
    int indeksLayanan = pilihDariMenu(daftarLayanan, "Pilih layanan: ");
    PilihanHarga layanan = daftarLayanan[indeksLayanan];

    long long hargaWisata =
        (destinasi.hargaPerHari + layanan.harga) * durasiHari;

    cout << "\nRincian wisata:\n";
    cout << "Destinasi: " << destinasi.nama << " (" << destinasi.jenisWisata << ")\n";
    cout << "Durasi: " << durasiHari << " hari\n";
    cout << "Layanan: " << layanan.nama << '\n';
    cout << "Harga wisata per peserta: " << formatRupiah(hargaWisata) << '\n';

    int jumlahPeserta = static_cast<int>(
        bacaAngka("\nMasukkan jumlah peserta (1-1000): ", 1, 1000));

    long long budget = 0;
    bool budgetSudahDiinput = false;
    long long totalBiaya = 0;
    PilihanHarga paketTerpilih = {"", 0};
    vector<string> tambahanTerpilih;

    while (true) {
        cout << "\nPilihan paket (harga per peserta per hari):\n";
        int indeksPaket = pilihDariMenu(daftarPaket, "Pilih paket: ");
        PilihanHarga paket = daftarPaket[indeksPaket];
        paketTerpilih = paket;

        long long hargaTambahan = paket.harga * durasiHari;
        RincianTambahan rincianTambahan = pilihTambahan();
        hargaTambahan += rincianTambahan.harga;
        tambahanTerpilih = rincianTambahan.nama;
        totalBiaya = (hargaWisata + hargaTambahan) * jumlahPeserta;

        cout << "\nTotal biaya untuk " << jumlahPeserta << " peserta: "
             << formatRupiah(totalBiaya) << '\n';

        if (!budgetSudahDiinput) {
            budget = bacaAngka("Masukkan budget: ", 1, 1000000000000LL);
            budgetSudahDiinput = true;
        } else {
            cout << "Budget: " << formatRupiah(budget) << '\n';
        }

        if (totalBiaya <= budget) {
            cout << "Budget mencukupi.\n";
            break;
        }

        cout << "Total biaya tidak sesuai budget. Silakan pilih ulang paket dan tambahan.\n";
        long long biayaMinimum = hargaWisata * jumlahPeserta;
        if (biayaMinimum > budget) {
            cout << "Budget belum mencukupi biaya wisata dasar, bahkan tanpa paket "
                 << "dan tambahan. Pemesanan tidak dapat dilanjutkan.\n";
            return 0;
        }
    }

    vector<string> namaPeserta;
    namaPeserta.reserve(jumlahPeserta);

    for (int i = 0; i < jumlahPeserta; ++i) {
        string pesan;
        if (i == 0) {
            pesan = "\nNama pemesan sekaligus peserta ke-1: ";
        } else {
            pesan = "Nama peserta ke-" + to_string(i + 1) + ": ";
        }
        namaPeserta.push_back(bacaTeks(pesan));
    }
    const string& namaPemesan = namaPeserta.front();

    const vector<string> metodePembayaran = {
        "Transfer bank", "QRIS", "Tunai"
    };
    bool pembayaranBerhasil = false;
    string metodeDipilih;

    while (!pembayaranBerhasil) {
        cout << "\nMetode pembayaran:\n";
        for (size_t i = 0; i < metodePembayaran.size(); ++i) {
            cout << i + 1 << ". " << metodePembayaran[i] << '\n';
        }

        int indeksMetode = static_cast<int>(bacaAngka(
            "Pilih metode pembayaran: ", 1, metodePembayaran.size())) - 1;
        metodeDipilih = metodePembayaran[indeksMetode];

        cout << "Memproses pembayaran " << formatRupiah(totalBiaya)
             << " melalui " << metodeDipilih << "...\n";
        pembayaranBerhasil = bacaKonfirmasi(
            "Simulasi status dari kanal pembayaran berhasil? (y/t): ");

        if (!pembayaranBerhasil) {
            cout << "Pembayaran gagal. Silakan coba metode pembayaran kembali.\n";
        }
    }

    bool dataTersimpan = simpanPemesanan(
        namaPemesan, namaPeserta, destinasi, durasiHari, layanan,
        paketTerpilih, tambahanTerpilih, budget, totalBiaya, metodeDipilih);

    cout << "\n========================================\n";
    cout << "PEMBAYARAN BERHASIL - E-VOUCHER\n";
    cout << "========================================\n";
    cout << "Nama pemesan: " << namaPemesan << '\n';
    cout << "Destinasi: " << destinasi.nama << '\n';
    cout << "Jumlah peserta: " << jumlahPeserta << '\n';
    cout << "Total dibayar: " << formatRupiah(totalBiaya) << '\n';
    cout << "Metode pembayaran: " << metodeDipilih << '\n';
    if (dataTersimpan) {
        cout << "Data pemesanan tersimpan di pemesanan_paket_wisata.txt\n";
    } else {
        cout << "Data pemesanan gagal disimpan ke file.\n";
    }

    cout << "\nDaftar peserta:\n";
    for (size_t i = 0; i < namaPeserta.size(); ++i) {
        cout << i + 1 << ". " << namaPeserta[i] << '\n';
    }

    cout << "\nITINERARY:\n";
    for (int hari = 1; hari <= durasiHari; ++hari) {
        cout << "Hari " << hari << ": wisata " << destinasi.jenisWisata
             << " di " << destinasi.nama << " dengan layanan "
             << layanan.nama << ".\n";
    }

    cout << "\nDOKUMEN PERJALANAN YANG PERLU DISIAPKAN:\n";
    cout << "- E-voucher pemesanan\n";
    cout << "- Kartu identitas setiap peserta\n";
    cout << "- Bukti pembayaran\n";

    cout << "\nPERSIAPAN KEBERANGKATAN:\n";
    cout << "- Periksa kembali jadwal dan barang bawaan.\n";
    cout << "- Pastikan semua peserta membawa dokumen perjalanan.\n";
    cout << "- Ikuti informasi keberangkatan dari penyelenggara.\n";

    cout << "\nPerjalanan wisata dilaksanakan. Selamat berwisata!\n";
    cout << "Pemesanan selesai.\n";
    return 0;
}
