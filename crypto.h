#ifndef _NAC_CRYPTO_H_
#define _NAC_CRYPTO_H_

#include <string>
using std::string;

void Encrypt(const string& text, string& encText, string& encSecKey);

#endif