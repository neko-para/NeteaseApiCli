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

	inline void add(rapidjson::Document& doc, const char* key, const string& s) {
		add(doc, key, s.c_str());
	}

	template <typename Type>
	inline void build(rapidjson::Document& doc, const char* key, const Type& value) {
		add(doc, key, value);
	}

	template <typename Type, typename... Arg>
	inline void build(rapidjson::Document& doc, const char* key, const Type& value, const Arg&... arg) {
		add(doc, key, value);
		build(doc, arg...);
	}

	string album(int id) {
		rapidjson::Document data;
		data.SetObject();
		return createWebAPIRequest("/weapi/v1/album/" + tostr(id), POST, data, cookie, cookie);
	}

	string artist(int id) {
		rapidjson::Document data;
		data.SetObject();
		return createWebAPIRequest("/weapi/v1/artist/" + tostr(id), POST, data, cookie, cookie);
	}

	string search(const string& keyword, SearchType type, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "type", type, "limit", limit, "offset", offset, "s", keyword);
		return createWebAPIRequest("/weapi/search/get", POST, data, cookie, cookie);
	}

	string song_detail(int id) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "c", R"([{"id":)" + tostr(id) + "}]", "ids", "[" + tostr(id) + "]");
		return createWebAPIRequest("/weapi/v3/song/detail", POST, data, cookie, cookie);
	}

	string user_detail(int id) {
		rapidjson::Document data;
		data.SetObject();
		return createWebAPIRequest("/weapi/v1/user/detail/" + tostr(id), POST, data, cookie, cookie);
	}

	string user_playlist(int id, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "limit", limit, "offset", offset, "uid", id);
		return createWebAPIRequest("/weapi/user/playlist", POST, data, cookie, cookie);
	}

}