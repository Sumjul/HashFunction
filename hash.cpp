#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <chrono>
using namespace std;

string ProgramStart() {
    string input;
    cout << "0 - jeigu norite irasyti teksta" << endl;
    cout << "1 - jeigu norite atidaryti faila" << endl;
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (choice == 0) {
        cout << "Iveskite teksta: " << endl;
        getline(cin, input);
    }
    else if (choice == 1) {
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
    return input;
}

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

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    cout << "Hash funkcijos veikimo laikas: " << duration.count() << " microseconds" << endl;

    char out [65];
    snprintf(out, sizeof(out), "%016llx%016llx%016llx%016llx", out4[0], out4[1], out4[2], out4[3]);
    return string(out);
}

int main()
{
    string input = ProgramStart();
    cout << "Hash funkcija: " << HashFun(input) << endl;
    return 0;
}
