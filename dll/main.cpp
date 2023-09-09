#include<iostream>
#include<string>
#include<vector>

using namespace std;

string getBinaryString(int n){
    string res = "";
    // cout << n << " " << char(n);
    for(int i = 0; i < 8; i++){
        res += '0' + n%2;
        n /= 2;
    }

    // cout << " " << res << endl;
    return string(res.rbegin(), res.rend());
}

void printHamming(string s){
    int n = 1;
    for(int i = 0; i < s.length(); i++){
        if(i == n-1) {
            cout << "\033[1;32m" << s[i] << "\033[1;0m";
            n *= 2;
        }
        else cout << s[i] ;
    }
    cout << endl;
}

// returns string with parity bit of hamming code equal to zero
string getWithZeroParityString(string s){
    string hamming_row = "";
    int next_power = 1;
    int cur = 0;
    for(int j = 0;  j < s.length(); j++){
        if(cur == 0){
            // power of 2
            hamming_row.push_back('0');
            cur = next_power;
            next_power *= 2;
            j--; // point to same char
        }else{
            hamming_row.push_back(s[j]);
        }
        cur--;
    }

    return hamming_row;
}

string getHammingString(string s){
    string hamming = getWithZeroParityString(s);

    int n_parity = hamming.length() - s.length();
    vector<bool> parity(n_parity, 0);

    for(int i = 0; i < hamming.length(); i++){
        for(int j = 0; j < parity.size(); j++){
            // cout << i+1 << ' ' << j << " "  <<  ((1 << j) & (i+1)) << endl;
            if( ((1 << j) & (i+1)) != 0){
                parity[j] = parity[j] == (hamming[i] - '0') ? 0 : 1;
            }
        }
    }

    for(int i = 0; i < parity.size(); i++){
        if(parity[i]){
            hamming[(1<<i) - 1] = '1';
        }
    }
    
    return hamming;
}

int main(){
    string data;
    int m;
    double prob;
    string generator;

    getline(std::cin, data);
    cin >> m >> prob >> generator ;

    cout << "Data String: " << data << endl;
    cout << "Data bytes: " << m << endl;
    cout << "probabilities: " << prob << endl;
    cout << "Generator: " << generator << endl;

    string padded = data + string(data.length()%m, '~');
    cout << "Data padded: " << padded << endl << endl;

    vector<string > columns;
    for(int i = 0; i*m < padded.size();i++){
        string row = "";
        for(int j = 0; j < m;j++){
            row += getBinaryString(padded[i*m + j]);
        }
        columns.push_back(row);
    }

    cout << "Data blocks" << endl;
    for(auto i : columns) cout << i << endl; cout << endl;

    cout << "After adding hamming codes: " << endl;
    vector<string> hamming_columns;
    for(auto &i : columns){
        string s = getHammingString(i);
        printHamming(s);
        hamming_columns.push_back(s);
    }

    string serialized = "";
    for(int i = 0; i < hamming_columns[0].length();i++){
        for(int j = 0; j < hamming_columns.size(); j++){
            serialized += hamming_columns[j][i] ;
        }
    }

    cout << endl;
    cout << "Data bits after column-wise serialization:" << endl;
    cout << serialized << endl;
}