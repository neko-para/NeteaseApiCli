#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <gmp.h>
using std::string;

string modulus = "00e0b509f6259df8642dbc35662901477df22677ec152b5ff68ace615bb7b725152b3ab17a876aea8a5aa76d2e417629ec4ee341f56135fccf695280104e0312ecbda92557c93870114af6c9d05c4f7f0c3685b7a46bee255932575cce10b424d813cfe4875d3e82047b97ddef52741d546b8e289dc6935b3ece0462db0a22b8e7";
string nonce = "0CoJUm6Qyw8W8jud";
string pubKey = "010001";

string trans(byte ch) {
	auto hex = [](byte i) -> char {
		if (i < 10) {
			return i + '0';
		} else {
			return i - 10 + 'a';
		}
	};
	char s[3] = { hex(ch >> 4), hex(ch & 0xF), 0 };
	return s;
}

string hexEncode(const string& s) {
	string ret;
	for (auto c : s) {
		ret += trans(c);
	}
	return ret;
}

string createSecretKey(int size) {
	const static char keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const static int keyslen = sizeof(keys) / sizeof(keys[0]);
	string key;
	for (int i = 0; i < size; ++i) {
		key += keys[rand() % keyslen];
	}
	return key;
}

static char base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64Encode(const string& text) {
	int len = text.length() / 3;
	string rst;
	unsigned data;
	string::const_iterator pos = text.cbegin();
	for (int i = 0; i < len; ++i) {
		data = (byte)*pos++;
		data = (data << 8) | (byte)*pos++;
		data = (data << 8) | (byte)*pos++;
		rst += base64Table[data >> 18];
		rst += base64Table[(data >> 12) & 63];
		rst += base64Table[(data >> 6) & 63];
		rst += base64Table[data & 63];
	}
	switch (text.length() % 3) {
		case 1:
			rst += base64Table[(byte)*pos >> 2];
			rst += base64Table[((byte)*pos << 4) & 63];
			rst += "==";
			break;
		case 2:
			data = (byte)*pos++;
			data = (data << 8) | (byte)*pos++;
			rst += base64Table[data >> 10];
			rst += base64Table[(data >> 4) & 63];
			rst += base64Table[(data << 2) & 63];
			rst += "=";
			break;
	}
	return rst;
}

string aesEncrypt(const string& text, const string& secKey) {
	string cipherText;
	const byte iv[] = "0102030405060708";
	CryptoPP::AES::Encryption aesEncryption((const byte*)secKey.c_str(), 16);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(cipherText));
	stfEncryptor.Put((const byte*)text.c_str(), text.length());
	stfEncryptor.MessageEnd();
	return base64Encode(cipherText);
}

void zfill(string& str, size_t size) {
	if (str.length() < size) {
		str = string(size - str.length(), '0');
	}
}



int main() {
	std::cout << aesEncrypt("Hello World!", "EWpXau7TtbEE6vNY");
}