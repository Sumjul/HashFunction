#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <chrono>
#include <random>
#include <iomanip>
#include <functional>
#include <openssl/sha.h>
using namespace std;

string HashFun(const string& str)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    unsigned long long h[8] = {0x5FAF3C1BULL, 0x6E8D3B27ULL, 0xA1C5E97FULL, 0x4B7D2E95ULL, 0xF2A39C68ULL, 0x3E9B5A7CULL, 0x9D74C5A1ULL, 0x7C1A5F3EULL };
    for (size_t ind = 0; ind < str.size(); ++ind) {
        unsigned char cByte = str[ind];
        size_t i = ind % 8;
        h[i] ^= ((h[(i+1)%8] << 7) | (h[(i+7)%8] >> 3));
        h[i] += cByte * 131 + (h[(i+3)%8] ^ h[(i+5)%8]);
    }

    for (int i = 0; i < 64; ++i) {
        size_t j = i % 8;
        h[j] ^= ((h[(j+1)%8] << ((i*7)%61)) | (h[(j+7)%8] >> ((i*5)%53)));
        h[j] += (h[(j+3)%8] ^ h[(j+5)%8]) + (0x9E3779B97F4A7C15ULL ^ (i * 0xA1C52E95ULL));
    }

    unsigned long long out4[4];
    for (int i = 0; i < 4; ++i) {
        out4[i] = h[i] ^ (h[i+4] << 1) ^ (h[(i+2)%8] >> 1);
    }

    char out [65];
    snprintf(out, sizeof(out), "%016llx%016llx%016llx%016llx", out4[0], out4[1], out4[2], out4[3]);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    //cout << "HashFun veikimo laikas: " << duration.count() << " microseconds" << endl;

    return string(out);
}

string sha256(const string &input)
{
    auto start = std::chrono::high_resolution_clock::now();

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)input.c_str(), input.size(), hash);
    ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    //cout << "SHA-256 veikimo laikas: " << duration.count() << " microseconds" << endl;

    return oss.str(); 
}

string RandomString(size_t len)
{
    static const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static mt19937_64 rng{random_device{}()};
    static uniform_int_distribution<size_t> dist (0, chars.size()-1);

    string s;
    s.reserve(len);
    for (size_t i=0; i<len; ++i) {
        s += chars[dist(rng)];
    }
    return s;
}

void Collision(size_t len)
{
    size_t collisions = 0;
    size_t collisionsSha = 0;
    for (size_t i = 0; i < 100000; ++i) {
        string a = RandomString(len);
        string b = RandomString(len);
        //cout << a << " " << b << " | " << HashFun(a) << " " << HashFun(b) << " " << sha256(a) << " " << sha256(b) << endl;

        if (HashFun(a) == HashFun(b))
            ++collisions;

        if (sha256(a) == sha256(b))
            ++collisionsSha;
    }
    cout << "100 000 porose rasta " << collisions << " koliziju (HashFun)" << endl;
    cout << "100 000 porose rasta " << collisionsSha << " koliziju (SHA-256)" << endl;
}

void AvalancheTest(std::function<string(const string&)> Fun)
{
    size_t len = 100;
    size_t minBits = SIZE_MAX, maxBits = 0, sumBits = 0;
    size_t minHex = SIZE_MAX, maxHex = 0, sumHex = 0;

    static const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < 100000; ++i) {
        string a = RandomString(len);
        string b = a;
        size_t pos = rand() % len;
        char newChar;
        do {
            newChar = chars[rand() % chars.size()];
        }
        while (newChar == b[pos]);
        b[pos] = newChar;

        string ha = Fun(a);
        string hb = Fun(b);
        //cout << a << " " << b << " | " << ha << " " << hb << endl;

        size_t bitDif = 0;
        for (size_t j = 0; j < ha.size(); ++j) {
            unsigned char x = static_cast<unsigned char>(ha[j]);
            unsigned char y = static_cast<unsigned char>(hb[j]);
            unsigned char dif = x^y;
            int c = 0;
            unsigned char v = dif;
            while (v) {
                c += v & 1;
                v >>= 1;
            }
            bitDif += c;
        }
        size_t hexDif = 0;
        for (size_t j = 0; j < ha.size(); ++j) {
            if (ha[j] != hb[j]) ++hexDif;
        }

        minBits = min(minBits, bitDif);
        maxBits = max(maxBits, bitDif);
        sumBits += bitDif;
        minHex = min(minHex, hexDif);
        maxHex = max(maxHex, hexDif);
        sumHex += hexDif;
    }
    cout << "Bitai: min " << (minBits / 256.0) * 100.0 << "%, max " << (maxBits / 256.0) * 100.0 << "%, vidurkis " << (sumBits / 100000.0 / 256.0) * 100.0 << "%" << endl;
    cout << "Hex'ai: min " << (minHex / 64.0) * 100.0 << "%, max " << (maxHex / 64.0) * 100.0 << "%, vidurkis " << (sumHex / 100000.0 / 64.0) * 100.0 << "%" << endl;
}

void HidingTest()
{
    string input;
    cout << "Iveskite slaptazodi: ";
    getline(cin, input);

    string salt = RandomString(16);
    string combined = input + salt;
    string hash1 = HashFun(combined);
    string hash2 = sha256(combined);

    cout << "Input: " << input << endl;
    cout << "Salt: " << salt << endl;
    cout << "HashFun (input+salt): " << hash1 << endl;
    cout << "SHA-256 (input+salt): " << hash2 <<endl;
}

string ProgramStart()
{
    string input;
    cout << "0 - jeigu norite irasyti teksta" << endl;
    cout << "1 - jeigu norite atidaryti faila" << endl;
    cout << "2 - koliziju testas" << endl;
    cout << "3 - lavinos efekto testas" << endl;
    cout << "4 - negriztamumo testas" << endl;
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (choice == 0) {
        cout << "Iveskite teksta: " << endl;
        getline(cin, input);
    }
    if (choice == 1) {
        cout << "Iveskite failo pavadinima: " << endl;
        string fileName;
        getline(cin, fileName);
        ifstream file(fileName);
        if (!file) {
            cerr << "Failas neatsidarė!\n";
            return 0;
        }
        stringstream buffer;
        buffer << file.rdbuf();
        input = buffer.str();
    }
    if (choice == 2) {
        cout << "Pasirinkite string'o ilgi (10/50/100/1000): " << endl;
        size_t len;
        cin >> len;
        if (len!=10 && len!=50 && len!=100 && len!=1000) {
            cerr << "Netinkamas ilgis" << endl;
            return {};
        }
        Collision(len);
    }
    if (choice == 3) {
        cout << "--- HashFun rezultatai: " << endl;
        AvalancheTest(HashFun);
        cout << "--- SHA-256 rezultatai: " << endl;
        AvalancheTest(sha256);
    }
    else if (choice == 4) {
        HidingTest();
    }
    return input;
}

int main()
{
    string input = ProgramStart();
    if (input != "") {
    cout << "Hashfun rezultatas: " << HashFun(input) << endl;
    cout << "SHA-256 rezultatas: " << sha256(input) << endl;
    }
    return 0;
}
