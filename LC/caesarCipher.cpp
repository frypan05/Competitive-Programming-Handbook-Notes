#include <bits/stdc++.h>
using namespace std;

string caesarCipher(string text, int shift, bool encode = true){
  string result = "";
  if (!encode) {
      shift = -shift;
  }//This is reverse shift for decoding.

  for (char &ch : text) {
    if (isalpha(ch)) {
      char shift_base = isupper(ch) ? 'A':'a';
      result += (ch - shift_base + shift + 26) % 26 + shift_base;
    } else{
      result += ch; //Keeping non-alphabet characters unchanged
    }
  }
  return result;
}

int main(){
  string message;
  int shift_value = 3;

  cout << "Enter the message to cipher: ";
  getline(cin, message);
  

  //This is going to give us the encoded text
  string encoded_message = caesarCipher(message,shift_value, true);
  cout << "Encoded: "<< encoded_message << endl;
  

  //This is going to give us the decoded text
  string decoded_message = caesarCipher(encoded_message, shift_value, false);
  cout << "Decoded: "<< decoded_message << endl;

  return 0;
}
