#include <algorithm>
#include <numeric>
#include <vector> 
#include <list>
#include <string>
#include <tuple>
#include <set>
#include <array> 
#include <iterator>
#include <random>
#include <array>
#include <iostream> 
#include <sstream>
#include <cmath>
#include <fstream> 
#include <cstdint>
#include <random> 
#include <unordered_map>

using namespace std;
using std::cout;
using std::endl;


  auto encrypted_text =
  "wb o gaozz jwzzous bsghzsr oawrgh gdfokzwbu twszrg, hvsfs zwjsr o viapzs tofasf boasr shvob. vwg rowzm fcihwbs fsjczjsr ofcibr hsbrwbu hc vwg qfcdg obr qofwbu tcf vwg obwaozg. sjsfm acfbwbu, vs fcgs sofzm, sousf hc sapfoqs hvs sbsfum ct hvs sofhv. kwhv ibkojsfwbu rsrwqohwcb, vs dzcksr hvs zobr, gcksr hvs gssrg, obr bifhifsr hvsa kwhv hsbrsf qofs. sjsbwbug ksfs gdsbh wb qcbhsbhasbh, og vs kohqvsr hvs gib usbhzm rsgqsbr psvwbr hvs rwghobh vwzzg. vwg zcjs tcf vwg zobr obr whg pcibhwtiz cttsfwbug ybsk bc pcibrg. shvob'g gwadzs zwts slsadzwtwsr hvs sggsbqs ct vofr kcfy, vcds, obr vofacbm kwhv bohifs."s;

  //write a function to find frequency of each letter and determine the key from the highest frequency. print the value of key. int find_key(const string &s);
  // write the encryptoon function to decrypt the message from the output of find_key() and print the decrypted message. void encrypt(const string &s, int key);
int find_key(const string &s) {
    array<int, 26> freq = {0}; // frequency array for letters a-z
    for (char ch : s) {
        if (isalpha(ch)) { // check if character is a letter
            freq[tolower(ch) - 'a']++; // increment frequency count
        }
    }
    int max_freq = 0;
    int key = 0;
    for (int i = 0; i < 26; ++i) {
        if (freq[i] > max_freq) {
            max_freq = freq[i];
            key = i;
        }
    }
    // Assuming 'e' is the most common letter in English text
    int assumed_e_index = 'e' - 'a';
    int decryption_key = (key - assumed_e_index + 26) % 26; // calculate decryption key
    cout << "Decryption key: " << decryption_key << endl;
    return decryption_key;
}

void encrypt(const string &s, int key) {
    string encrypted;
    for (char ch : s) {
        if (isalpha(ch)) {
            char base = islower(ch) ? 'a' : 'A';
            encrypted += (ch - base + key) % 26 + base;
        } else {
            encrypted += ch;
        }
    }
    cout << "Encrypted message: " << encrypted << endl;
}

int main
() {
    int key = find_key(encrypted_text);
    encrypt(encrypted_text, key);
    return 0;
}