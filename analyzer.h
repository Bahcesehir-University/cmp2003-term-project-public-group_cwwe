#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <array>

//  istenen temel yapılar
struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
private:
    // Bölgelerin toplam sayısını tutan map (Zone -> Sayı)
    std::unordered_map<std::string, long long> zoneCounts;
    
    // Her bölge için 24 saatlik dizi tutan map (Zone -> [0-23 saatler])
    // Bu yapı hız için en mantıklısı olduğu için bunu seçtim.
    std::unordered_map<std::string, std::array<long long, 24>> slotCounts;

public:
    // Dosyadan veri okuma
    void ingestFile(const std::string& csvPath);
    
    // HackerRank için gerekli
    void ingestStdin();

    // En yoğun k bölgeyi getiren fonksiyon
    std::vector<ZoneCount> topZones(int k = 10) const;
    
    // En yoğun k zaman dilimini getiren fonksiyon
    std::vector<SlotCount> topBusySlots(int k = 10) const;
};

#endif