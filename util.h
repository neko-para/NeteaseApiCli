#ifndef __UTIL_H_
#define __UTIL_H_

#include <rapidjson/document.h>
#include <string>
using std::string;

enum {
	GET,
	POST
};

string createWebAPIRequest(const string& path, int method, rapidjson::Document& data, const string& cookie, string& retcookie);

#endif