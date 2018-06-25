#include "NeteaseApi.h"
#include <curl/curl.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <vector>

const char* help = 
R"(NeteaseApi cmd params...
usage:
	~ help
	~ album id(%d)
	~ artist id(%d)
	~ music.url id(%d) [bitrate(%d)=999000]
	~ search text [type(song, album, artist, playlist, user)=song [limit(%d)=30 [offset(%d)=0]]]
	~ song.detail id(%d)
	~ user.detail id(%d)
	~ user.playlist id(%d) [limit(%d)=30 [offset(%d)=0]]
)";

#define paramis(id, text) !strcmp(arg(id), #text)

size_t appendData(void* ptr, size_t size, size_t nmemb, void* user) {
	std::vector<char>* p = (std::vector<char>*)user;
	auto cs = p->size();
	p->resize(cs + size * nmemb);
	memcpy(p->data() + cs, ptr, size * nmemb);
	return size * nmemb;
}

using netease::Action;

string download(const Action& action) {
	std::vector<char> retdata;
	CURL* curl = curl_easy_init();
	curl_slist* header = 0;
	for (const auto& s : netease::headers) {
		header = curl_slist_append(header, s.c_str());
	}
	header = curl_slist_append(header, ("User-Agent: " + netease::agents[rand() % netease::agents_count]).c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_URL, action.url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, action.post.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, action.post.length());
	//curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
	curl_easy_perform(curl);
	curl_slist* cookieList;
    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookieList);
	for (auto p = cookieList; p; p = p->next) {
		std::cerr << p->data << std::endl; // TODO: return cookie
	}
	curl_slist_free_all(cookieList);
	curl_slist_free_all(header);
	curl_easy_cleanup(curl);
	retdata.push_back(0);
	return retdata.data();
}

int main(int argc, char* argv[]) {
	srand(time(0));
	auto arg = [=](int i) -> char* {
		if (argc <= i) {
			throw "";
		} else {
			return argv[i];
		}
	};
	curl_global_init(CURL_GLOBAL_ALL);
	try {
		if (paramis(1, help)) {
			std::cout << help;
		} else if (paramis(1, album)) {
			std::cout << download(netease::album(atoi(arg(2))));
		} else if (paramis(1, artist)) {
			std::cout << download(netease::artist(atoi(arg(2))));
		} else if (paramis(1, music.url)) {
			int bitrate = 999000;
			try {
				bitrate = atoi(arg(3));
			} catch (...) {}
			std::cout << download(netease::music_url(atoi(arg(2)), bitrate));
		} else if (paramis(1, search)) {
			netease::SearchType st = netease::ST_SONG;
			int limit = 30, offset = 0;
			try {
				if (paramis(3, song)) {
					st = netease::ST_SONG;
				} else if (paramis(3, album)) {
					st = netease::ST_ALBUM;
				} else if (paramis(3, artist)) {
					st = netease::ST_ARTIST;
				} else if (paramis(3, playlist)) {
					st = netease::ST_PLAYLIST;
				} else if (paramis(3, user)) {
					st = netease::ST_USER;
				}
				limit = atoi(arg(4));
				offset = atoi(arg(5));
			} catch (...) {}
			std::cout << download(netease::search(arg(2), st, limit, offset));
		} else if (paramis(1, song.detail)) {
			std::cout << download(netease::song_detail(atoi(arg(2))));
		} else if (paramis(1, user.detail)) {
			std::cout << download(netease::user_detail(atoi(arg(2))));
		} else if (paramis(1, user.playlist)) {
			int limit = 30, offset = 0;
			try {
				limit = atoi(arg(3));
				offset = atoi(arg(4));
			} catch (...) {}
			std::cout << download(netease::user_playlist(atoi(arg(2)), limit, offset));
		}
	} catch (...) {
		std::cout << help;
		return 1;
	}
	curl_global_cleanup();
}