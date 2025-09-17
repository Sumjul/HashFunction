#include <iostream>
#include <string>
using namespace std;

string HashFun(const string& str) {
    unsigned long long h[4] = {0x12345678ULL, 0x9ABCDEF0ULL, 0x0FEDCBA9ULL, 0x87654321ULL};
    for (size_t i = 0; i < str.size(); ++i) {
        unsigned char cByte = str[i];
        h[i % 4] ^= (h[i % 4] << 5) + (h[i % 4] >> 2) + cByte;
    }
    char out [65];
    snprintf(out, sizeof(out), "%016llx%016llx%016llx%016llx", h[0], h[1], h[2], h[3]);
    return string(out);
}

int main()
{
    string input;
    cout << "Iveskite bet koki teksta:" << endl;
    getline(cin, input);
    cout << HashFun(input) << endl;
    return 0;
}
