#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <iomanip>
#include <functional>
#include <limits>
#include <openssl/sha.h>
using namespace std;

string HashFun(const string& str) {
    auto start = chrono::high_resolution_clock::now();
    
    unsigned long long seed[8] = {0x5FAF3C1BULL, 0x6E8D3B27ULL, 0xA1C5E97FULL, 0x4B7D2E95ULL, 0xF2A39C68ULL, 0x3E9B5A7CULL, 0x9D74C5A1ULL, 0x7C1A5F3EULL};
    for (size_t i = 0; i < str.size(); ++i) {
        unsigned char cByte = str[i];
        size_t ind = i % 8;
        seed[ind] ^= ((seed[(ind + 1) % 8] << 7) | (seed[(ind + 7) % 8] >> 3));
        seed[ind] += cByte * 131 + (seed[(ind + 3) % 8] ^ seed[(ind + 5) % 8]);
    }

    for (int i = 0; i < 64; ++i) {
        size_t ind = i % 8;
        seed[ind] ^= ((seed[(ind + 1) % 8] << ((i * 7) % 61)) | (seed[(ind + 7) % 8] >> ((i * 5) % 53)));
        seed[ind] += (seed[(ind + 3) % 8] ^ seed[(ind + 5) % 8]) + (0x9E3779B97F4A7C15ULL ^ (i * 0xA1C52E95ULL));
    }

    unsigned long long out4[4];
    for (int i = 0; i < 4; ++i) {
        out4[i] = seed[i] ^ (seed[i + 4] << 1) ^ (seed[(i + 2) % 8] >> 1);
    }

    char out [65];
    snprintf(out, sizeof(out), "%016llx%016llx%016llx%016llx", out4[0], out4[1], out4[2], out4[3]);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end-start);
    //cout << "HashFun veikimo laikas: " << duration.count() << " microseconds" << endl;

    return string(out);
}

string sha256(const string &input) {
    auto start = chrono::high_resolution_clock::now();

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)input.c_str(), input.size(), hash);
    
    ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end-start);
    //cout << "SHA-256 veikimo laikas: " << duration.count() << " microseconds" << endl;

    return oss.str(); 
}

string RandomString(size_t len) {
    static const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static mt19937_64 rng{random_device{}()};
    static uniform_int_distribution<size_t> dist (0, chars.size()-1);

    string s;
    s.reserve(len);
    for (size_t i = 0; i < len; ++i) {
        s += chars[dist(rng)];
    }
    return s;
}

void CollisionTest(size_t len) {
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

void AvalancheTest(function<string(const string&)> Fun) {
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

        string aHash = Fun(a);
        string bHash = Fun(b);
        //cout << a << " " << b << " | " << ha << " " << hb << endl;

        size_t bitDif = 0;
        for (size_t j = 0; j < aHash.size(); ++j) {
            unsigned char x = static_cast<unsigned char>(aHash[j]);
            unsigned char y = static_cast<unsigned char>(bHash[j]);
            unsigned char dif = x ^ y;
            while (dif) {
                bitDif += dif & 1;
                dif >>= 1;
            }
        }
        size_t hexDif = 0;
        for (size_t j = 0; j < aHash.size(); ++j) {
            if (aHash[j] != bHash[j]) ++hexDif;
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

void HidingTest() {
    string input;
    cout << "Iveskite slaptazodi: ";
    getline(cin, input);

    string salt = RandomString(16);
    string combined = input + salt;

    cout << "Input: " << input << endl;
    cout << "Salt: " << salt << endl;
    cout << "HashFun (input+salt): " << HashFun(combined) << endl;
    cout << "SHA-256 (input+salt): " << sha256(combined) << endl;
}

string ProgramStart() {
    string input;
    cout << "0 - Irasyti teksta konsoleje" << endl;
    cout << "1 - Naudoti duomenis is tekstinio failo" << endl;
    cout << "2 - Koliziju paieska" << endl;
    cout << "3 - Lavinos efekto testas" << endl;
    cout << "4 - Negriztamumo demonstracija" << endl;

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
            cerr << "Failas neatsidarė!" << endl;
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
            cerr << "Netinkamas ilgis!" << endl;
            return 0;
        }
        CollisionTest(len);
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

int main() {
    cout << "-----------------------------------" << endl;
    string input = ProgramStart();
    //if (input != "")
    cout << "Hashfun rezultatas: " << HashFun(input) << endl << "SHA-256 rezultatas: " << sha256(input) << endl;
    cout << "-----------------------------------" << endl;
    return 0;
}