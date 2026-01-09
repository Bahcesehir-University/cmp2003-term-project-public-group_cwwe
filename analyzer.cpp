#include "analyzer.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// Satırı sütunlara ayırıp gerekli bilgileri çeken yardımcı fonksiyon.
// A2 testindeki eksik sütun hatasını çözmek için virgül sayısını kontrol ediyorum.
void satiri_isle(const std::string& satir, 
                 std::unordered_map<std::string, long long>& bolge_sayilari,
                 std::unordered_map<std::string, std::array<long long, 24>>& saatlik_slotlar) {
    
    if (satir.empty()) return;

    // Virgülleri tek tek buluyorum ki hocanın istediği 6 sütun (5 virgül) kuralına uysun.
    size_t v1 = satir.find(','); 
    size_t v2 = satir.find(',', v1 + 1);
    size_t v3 = satir.find(',', v2 + 1);
    size_t v4 = satir.find(',', v3 + 1);
    size_t v5 = satir.find(',', v4 + 1);

    // Eğer 5. virgül yoksa bu satır bozuktur, direkt geçiyorum (A2 kuralı).
    if (v1 == std::string::npos || v2 == std::string::npos || 
        v3 == std::string::npos || v4 == std::string::npos || 
        v5 == std::string::npos) return;

    // PickupZoneID v1 ile v2 arasındaymış.
    std::string bolge = satir.substr(v1 + 1, v2 - v1 - 1);
    if (bolge.empty()) return; // İsim yoksa saymıyorum.

    // Zaman bilgisi PickupDateTime ise v3 ile v4 arasında.
    std::string zaman_metni = satir.substr(v3 + 1, v4 - v3 - 1);

    // Format "YYYY-MM-DD HH:MM" şeklinde olduğu için boşluğu bulup yanındaki saati alıyorum.
    size_t bosluk = zaman_metni.find(' ');
    if (bosluk == std::string::npos) return;

    try {
        // Saatin olduğu kısmı (mesela "10") alıp sayıya çeviriyorum.
        int saat = std::stoi(zaman_metni.substr(bosluk + 1, 2));
        if (saat >= 0 && saat <= 23) {
            bolge_sayilari[bolge]++;
            saatlik_slotlar[bolge][saat]++;
        }
    } catch (...) {
        // stoi hata verirse satır hatalıdır, hiçbir şey yapmadan devam et.
    }
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream dosya(csvPath);
    if (!dosya.is_open()) return;

    std::string satir;
    // İlk satır başlık olduğu için onu bir kere okuyup atlıyorum.
    if (!std::getline(dosya, satir)) return;

    while (std::getline(dosya, satir)) {
        satiri_isle(satir, zoneCounts, slotCounts);
    }
    dosya.close();
}

void TripAnalyzer::ingestStdin() {
    std::string satir;
    // HackerRank için standart inputtan okuma. Başlığı yine atlıyoruz.
    if (!std::getline(std::cin, satir)) return;

    while (std::getline(std::cin, satir)) {
        satiri_isle(satir, zoneCounts, slotCounts);
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> sonuc;
    // Map'teki verileri sıralamak için vector'e kopyalıyorum.
    for (auto const& eleman : zoneCounts) {
        sonuc.push_back({eleman.first, eleman.second});
    }

    // Sıralama: Önce sayı (büyükten küçüğe), eşitlik varsa isim (A-Z).
    std::sort(sonuc.begin(), sonuc.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    });

    if ((int)sonuc.size() > k) sonuc.resize(k);
    return sonuc;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> sonuc;
    // Map içindeki array'i gezip dolu olan saatleri topluyorum.
    for (auto const& bolge_elemani : slotCounts) {
        for (int i = 0; i < 24; ++i) {
            if (bolge_elemani.second[i] > 0) {
                sonuc.push_back({bolge_elemani.first, i, bolge_elemani.second[i]});
            }
        }
    }

    // Sıralama kuralları: Sayı (desc), İsim (asc), Saat (asc).
    std::sort(sonuc.begin(), sonuc.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    });

    if ((int)sonuc.size() > k) sonuc.resize(k);
    return sonuc;
}