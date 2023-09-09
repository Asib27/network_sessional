#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>

using namespace std;

// Function to calculate the number of parity bits required
int calculateParityBits(int dataSize) {
    int m = dataSize;
    int r = 0;
    while ( (1<<r) < m + r + 1) {
        r++;
    }
    return r;
}

// Function to generate the Hamming code
string generateHammingCode(const string& input) {
    int dataSize = input.length();
    int r = calculateParityBits(dataSize);
    int hammingCodeSize = dataSize + r;
    vector<int> hammingCode(hammingCodeSize, 0);

    // Copy data bits into the Hamming code
    for (int i = 0, j = 0; i < hammingCodeSize; i++) {
        if (i + 1 != pow(2, j)) {
            hammingCode[i] = input[j] - '0';
            j++;
        }
    }

    // Calculate parity bits
    for (int i = 0; i < r; i++) {
        int parityBitIndex = pow(2, i) - 1;
        int parityBitValue = 0;

        for (int j = parityBitIndex; j < hammingCodeSize; j += (parityBitIndex + 1) * 2) {
            for (int k = j; k < j + parityBitIndex + 1 && k < hammingCodeSize; k++) {
                parityBitValue ^= hammingCode[k];
            }
        }

        hammingCode[parityBitIndex] = parityBitValue;
    }

    // Convert the Hamming code to a string
    string hammingCodeString = "";
    for (int i = 0; i < hammingCodeSize; i++) {
        hammingCodeString += to_string(hammingCode[i]);
    }

    return hammingCodeString;
}

int main() {
    string inputString = "Hello, World!";
    string binaryInput = "";

    // Convert the input string to binary representation
    for (char c : inputString) {
        binaryInput += bitset<8>(c).to_string();
    }

    string hammingCode = generateHammingCode(binaryInput);

    cout << "Input String: " << inputString << endl;
    cout << "Binary Representation: " << binaryInput << endl;
    cout << "Hamming Code: " << hammingCode << endl;

    return 0;
}
