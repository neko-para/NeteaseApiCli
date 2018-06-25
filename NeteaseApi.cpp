#include "NeteaseApi.h"
#include "crypto.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace netease {

	inline string tostr(int i) {
		char temp[12];
		sprintf(temp, "%d", i);
		return temp;
	}

	string tostr(const rapidjson::Document& doc) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);
		return sb.GetString();
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

	static string build_post(rapidjson::Document& doc) {
		add(doc, "csrf_token", "");
		string encText, encKey;
		Encrypt(tostr(doc), encText, encKey);
		return "params=" + UrlEncode(encText) + "&encSecKey=" + UrlEncode(encKey);
	}

	Action album(int id) {
		rapidjson::Document data;
		data.SetObject();
		return Action("/v1/album/" + tostr(id), build_post(data));
	}

	Action artist(int id) {
		rapidjson::Document data;
		data.SetObject();
		return Action("/v1/artist/" + tostr(id), build_post(data));
	}

	Action music_url(int id, int bitrate) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "ids", "[" + tostr(id) + "]", "br", bitrate);
		return Action("/song/enhance/player/url", build_post(data));
	}

	Action search(const string& keyword, SearchType type, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "type", type, "limit", limit, "offset", offset, "s", keyword);
		return Action("/search/get", build_post(data));
	}

	Action song_detail(int id) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "c", R"([{"id":)" + tostr(id) + "}]", "ids", "[" + tostr(id) + "]");
		return Action("/v3/song/detail", build_post(data));
	}

	Action user_detail(int id) {
		rapidjson::Document data;
		data.SetObject();
		return Action("v1/user/detail/" + tostr(id), build_post(data));
	}

	Action user_playlist(int id, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "limit", limit, "offset", offset, "uid", id);
		return Action("/user/playlist", build_post(data));
	}

}