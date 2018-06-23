#include "crypto.h"
#include "util.h"
#include <cstdlib>
#include <vector>
#include <regex>
#include <iostream>
#include <curl/curl.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

static string agents[] = {
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Safari/537.36",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 9_1 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13B143 Safari/601.1",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 9_1 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13B143 Safari/601.1",
	"Mozilla/5.0 (Linux; Android 5.0; SM-G900P Build/LRX21T) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Mobile Safari/537.36",
	"Mozilla/5.0 (Linux; Android 5.1.1; Nexus 6 Build/LYZ28E) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.115 Mobile Safari/537.36",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) Mobile/14F89;GameHelper",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_5) AppleWebKit/603.2.4 (KHTML, like Gecko) Version/10.1.1 Safari/603.2.4",
	"Mozilla/5.0 (iPhone; CPU iPhone OS 10_0 like Mac OS X) AppleWebKit/602.1.38 (KHTML, like Gecko) Version/10.0 Mobile/14A300 Safari/602.1",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:46.0) Gecko/20100101 Firefox/46.0",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:46.0) Gecko/20100101 Firefox/46.0",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0)",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)",
	"Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; Win64; x64; Trident/6.0)",
	"Mozilla/5.0 (Windows NT 6.3; Win64, x64; Trident/7.0; rv:11.0) like Gecko",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36 Edge/13.10586",
	"Mozilla/5.0 (iPad; CPU OS 10_0 like Mac OS X) AppleWebKit/602.1.38 (KHTML, like Gecko) Version/10.0 Mobile/14A300 Safari/602.1"
};

string randomUserAgent() {
	return agents[rand() % (sizeof(agents) / sizeof(agents[0]))];
}

size_t appendData(void* ptr, size_t size, size_t nmemb, void* user) {
	std::vector<char>* p = (std::vector<char>*)user;
	auto cs = p->size();
	p->resize(cs + size * nmemb);
	memcpy(p->data() + cs, ptr, size * nmemb);
	return size * nmemb;
}

string ToStr(const rapidjson::Document& doc) {
	rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);
	return sb.GetString();
}

string createWebAPIRequest(const string& path, int method, rapidjson::Document& data, const string& cookie, string& retcookie) {
	std::regex getCSRF(R"(/_csrf=[^(;|$)]+;/)");
	std::cmatch CSRFrst;
	if (std::regex_match(cookie.c_str(), CSRFrst, getCSRF)) {
		data.AddMember(rapidjson::Value("csrf_token"), rapidjson::Value(CSRFrst[0].str().c_str(), data.GetAllocator()), data.GetAllocator());
	} else {
		data.AddMember(rapidjson::Value("csrf_token"), rapidjson::Value(""), data.GetAllocator());
	}
	string encText, encKey;
	Encrypt(ToStr(data), encText, encKey);
	string body = "params=" + UrlEncode(encText) + "&encSecKey=" + UrlEncode(encKey);
	std::vector<char> retdata;
	CURL* curl = curl_easy_init();
	curl_slist* header = 0;
	header = curl_slist_append(header, "Accept: */*");
	header = curl_slist_append(header, "Accept-Language: zh-CN,zh;q=0.8,gl;q=0.6,zh-TW;q=0.4");
	header = curl_slist_append(header, "Connection: keep-alive");
	header = curl_slist_append(header, "Content-Type: application/x-www-form-urlencoded");
	header = curl_slist_append(header, "Referer: http://music.163.com");
	header = curl_slist_append(header, "Host: music.163.com");
	header = curl_slist_append(header, ("Cookie: " + cookie).c_str());
	header = curl_slist_append(header, ("User-Agent: " + randomUserAgent()).c_str());
	curl_easy_setopt(curl, CURLOPT_POST, method);
	curl_easy_setopt(curl, CURLOPT_URL, ("http://music.163.com" + path).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
	curl_easy_perform(curl);
    //curl_easy_getinfo(curl, CURLINFO_COOKIELIST, )
	curl_slist_free_all(header);
	curl_easy_cleanup(curl);
	retdata.push_back(0);
	return retdata.data();
}