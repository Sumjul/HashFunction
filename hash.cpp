#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

string HashFun(const string& str)
{
    unsigned long long h[8] = {0x12345678ULL, 0x9ABCDEF0ULL, 0x0FEDCBA9ULL, 0x87654321ULL, 0xA5A5A5A5ULL, 0xC3C3C3C3ULL, 0x5F5F5F5FULL, 0x1B1B1B1BULL};
    for (size_t ind = 0; ind < str.size(); ++ind) {
        unsigned char cByte = str[ind];
        size_t i = ind % 8;
        h[i] ^= ((h[(i+1)%8] << 7) | (h[(i+7)%8] >> 3));
        h[i] += cByte * 131 + (h[(i+3)%8] ^ h[(i+5)%8]);
    }

    for (int i = 0; i < 16; ++i) {
        size_t j = i % 8;
        h[j] ^= ((h[(j+1)%8] << 7) | (h[(j+7)%8] >> 3));
        h[j] += (h[(j+3)%8] ^ h[(j+5)%8]) + 0x9E3779B97F4A7C15ULL;
    }

    unsigned long long out4[4];
    for (int i = 0; i < 4; ++i) {
        out4[i] = h[i] ^ (h[i+4] << 1) ^ (h[(i+2)%8] >> 1);
    }

    char out [65];
    snprintf(out, sizeof(out), "%016llx%016llx%016llx%016llx", out4[0], out4[1], out4[2], out4[3]);
    return string(out);
}

int main()
{
    string input;
    cout << "Iveskite failo pavadinima:" << endl;
    string fileName;
    getline(cin, fileName);
    ifstream file(fileName);
    if (!file) {
        cerr << "Failas neatsidarė!\n";
        return 1;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    input = buffer.str();

    cout << "Hash funkcija: " << HashFun(input) << endl;
    return 0;
}


//   Tuscias failas: febfa30aad69de7e9263d38bb4a0163d6ff45d795e5fc444effa987d78b8fa0a
//    Failas su "1": feba450d5ac5f27b926667429c8edc2e6fb4335b46f5247aef2c0c5945be882a
// konstitucija.txt: ed4d7e0ce2417bbc914f6ced8486882b5059d5f5f000652982fefd9a3d40f477
// konst pakeistas s:fa759b550f70e445b872cfd93dfbfa139fe0ebd96f93f69d67eb13238c89f193