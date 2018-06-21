#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
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
	std::vector<byte> textdata(text.begin(), text.end());
	textdata.push_back(0);
	unsigned data = 0;
	int ignore = 2;
	string result;
	for (auto c : textdata) {
		data <<= 8;
		data |= (byte)c;
		result.push_back(base64Table[data >> ignore]);
		data &= (1 << ignore) - 1;
		if (ignore == 6) {
			result.push_back(base64Table[data]);
			data = 0;
			ignore = 0;
		}
		ignore += 2;
	}
	result.push_back('=');
	result.push_back('=');
	return result;
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

int main() {
	std::cout << aesEncrypt("Hello World!", "EWpXau7TtbEE6vNY");
}