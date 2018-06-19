#include <string>
#include <cmath>
#include <crypto++/aes.h>
using std::string;

string modulus = "00e0b509f6259df8642dbc35662901477df22677ec152b5ff68ace615bb7b725152b3ab17a876aea8a5aa76d2e417629ec4ee341f56135fccf695280104e0312ecbda92557c93870114af6c9d05c4f7f0c3685b7a46bee255932575cce10b424d813cfe4875d3e82047b97ddef52741d546b8e289dc6935b3ece0462db0a22b8e7";
string nonce = "0CoJUm6Qyw8W8jud";
string pubKey = "010001";

string hexEncode(const string& s) {
	auto hex = [](int i) -> char {
		if (i < 10) {
			return i + '0';
		} else {
			return i - 10 + 'a';
		}
	};
	auto trans = [hex](char ch) -> string {
		return string(1, hex(ch >> 4)) + string(1, hex(ch & 0xF));
	};
	string ret;
	for (auto ch : s) {
		ret += trans(ch);
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

