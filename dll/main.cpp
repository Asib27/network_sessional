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

string findXOR(string a, string b){
    string res = "";
    for(int i = 0; i < a.length(); i++){
        if(a[i] == b[i]) res.push_back('0');
        else res.push_back('1');
    }
    return res;
}

string getRemainder(string m, string gen, bool check=false){
    if(!check)
        m.append(string(gen.length()-1,'0'));

    string rem = m.substr(0, gen.length());
    for(int i = gen.length(); i < m.length(); i++){
        if(rem[0] == '1'){
            rem = findXOR(rem, gen);
        }
        rem = rem.substr(1) + m[i];
    }

    if(rem[0] == '0') return rem.substr(1);
    return findXOR(rem, gen).substr(1);
}

int main(){
    srand(1927);

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

    // padding data
    string padded = data + string( data.length()%m == 0? 0: m - data.length()%m, '~');
    cout << "Data padded: " << padded << endl << endl;

    // arranging into column
    int n_col = m*8;
    int n_row = padded.length() / m;

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

    // adding hamming code
    cout << "After adding hamming codes: " << endl;
    vector<string> hamming_columns;
    for(auto &i : columns){
        string s = getHammingString(i);
        printHamming(s);
        hamming_columns.push_back(s);
    }
    n_col = hamming_columns[0].length();

    // serializing
    string serialized = "";
    for(int i = 0; i < hamming_columns[0].length();i++){
        for(int j = 0; j < hamming_columns.size(); j++){
            serialized += hamming_columns[j][i] ;
        }
    }

    cout << endl;
    cout << "Data bits after column-wise serialization:" << endl;
    cout << serialized << endl;

    // crc checksum generate
    string crc_checksum =  getRemainder(serialized, generator) ;
    string sent = serialized + crc_checksum;
    cout << endl;
    cout << "Data bits after appending CRC checksum:" << endl;
    cout << serialized << "\033[1;36m" << crc_checksum << "\033[1;0m" << endl;

    // recieving data
    string recieved = "";
    vector<bool> errors;
    for(auto i: sent){
        int random = rand() % 1000000;
        if(random < prob * 1000000){
            recieved.push_back(i == '0'? '1':'0');
            cout << "\033[1;31m" << recieved.back() << "\033[1;0m";
            errors.push_back(1);
        }
        else{
            recieved.push_back(i);
            cout << i ;
            errors.push_back(0);
        }
    }
    cout << endl << endl;

    // checking checksum
    string error_check = getRemainder(recieved, generator, true);
    cout << "Result of CRC checksum matching: " 
        << (error_check.find('1') != -1? "error detected" : "No error detected")
        << endl << endl;

    // deserializing data
    vector<string> recieved_columns(n_row, string(n_col, '0'));
    vector<vector<bool>> error_columns(n_row, vector<bool>(n_col, 0));
    for(int i = 0; i < recieved.size(); i++){
        int row = i / n_row;
        int col = i % n_row;
        recieved_columns[col][row] = recieved[i];
        error_columns[col][row] = errors[i];
    }

    cout << "Data block after removeing CRC checksum bits: " << endl;
    for(int i = 0; i < n_row; i++){
        for (int j = 0; j < n_col; j++){ 
            if(error_columns[i][j]){
                cout << "\033[1;31m" << recieved_columns[i][j] << "\033[1;0m";
            }
            else
                cout << recieved_columns[i][j];
        }
        cout << endl;
    }
    cout << endl;
    
}