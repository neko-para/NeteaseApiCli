#include "NeteaseApi.h"
#include "util.h"
#include "crypto.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <rapidjson/document.h>

namespace netease {

	static string cookie;

	void init() {
		curl_global_init(CURL_GLOBAL_ALL);
		srand(time(0));
	}

	void exit() {
		curl_global_cleanup();
	}

	void setcookie(const string& c) {
		cookie = c;
	}

	string getcookie() {
		return cookie;
	}

	inline string tostr(int i) {
		char temp[12];
		sprintf(temp, "%d", i);
		return temp;
	}

	inline void add(rapidjson::Document& doc, const char* key, int i) {
		doc.AddMember(rapidjson::Value(key, doc.GetAllocator()), rapidjson::Value(tostr(i).c_str(), doc.GetAllocator()), doc.GetAllocator());
	}

	inline void add(rapidjson::Document& doc, const char* key, const char* s) {
		doc.AddMember(rapidjson::Value(key, doc.GetAllocator()), rapidjson::Value(s, doc.GetAllocator()), doc.GetAllocator());
	}

	string search(const string& keyword, SearchType type, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		add(data, "type", type);
		add(data, "limit", limit);
		add(data, "offset", offset);
		add(data, "s", keyword.c_str());
		return createWebAPIRequest("/weapi/search/get", POST, data, cookie, cookie);
	}

}