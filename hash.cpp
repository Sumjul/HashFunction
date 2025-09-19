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

// konstitucija.txt hash'as: 01df182769b0ab126f0fc9b87bcc1e3280f4288c0d52507ee5626f69c1a1df9d