#include "NeteaseApi.h"
#include <curl/curl.h>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <vector>
#include <termios.h>

bool rawprint = false;
string icookie = "/tmp/neteaseapicookie", ocookie = "/tmp/neteaseapicookie";

const char* version = "NeteaseApiCli 1.0.0 libNeteaseApi " NETEASEAPI_VERSION;

const char* help = 
R"(NeteaseApi poption] command params...
regular option:
	-r              print url and post data directly
	-h              generate a header
	-v              print version
cookie option:
	Note: default cookie file is /tmp/neteaseapicookie
	-a cookie-file  specify cookie file for both reading and writing
	-b cookie-file  specify cookie file for reading
	-c cookie-file  specify cookie file for writing

command:
	Note: command with @ means this method need login.
	~ help
	~ album id(%d)
	~ artist id(%d)
	~ music.url id(%d) [bitrate(%d)=999000]
	~ login.cellphone phone(%d) <password
	@ personal.fm
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

inline string randomUserAgent() {
	return netease::agents[rand() % netease::agents_count];
}

string download(const Action& action) {
	std::vector<char> retdata;
	CURL* curl = curl_easy_init();
	curl_slist* header = 0;
	for (const auto& s : netease::headers) {
		header = curl_slist_append(header, s.c_str());
	}
	header = curl_slist_append(header, ("User-Agent: " + randomUserAgent()).c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_URL, action.url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, action.post.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, action.post.length());
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, icookie.c_str());
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, ocookie.c_str());
	curl_easy_perform(curl);
	curl_slist_free_all(header);
	curl_easy_cleanup(curl);
	retdata.push_back(0);
	return retdata.data();
}

string print(const Action& a) {
	if (rawprint) {
		return a.url + "\n" + a.post + "\n";
	} else {
		return download(a);
	}
}

void close_echo() {
	struct termios opt;
	tcgetattr(fileno(stdin), &opt);
	opt.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSAFLUSH, &opt);
}

void open_echo() {
	struct termios opt;
	tcgetattr(fileno(stdin), &opt);
	opt.c_lflag |= ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &opt);
}

int main(int argc, char* argv[]) {
	int deal = 0;
	srand(time(0));
	auto arg = [&](int i) -> char* {
		if (argc <= deal + i) {
			throw "";
		} else {
			return argv[deal + i];
		}
	};
	try {
		while (true) {
			auto p = arg(1);
			if (p[0] == '-') {
				switch (p[1]) {
				case 'a':
					icookie = ocookie = arg(2);
					deal += 2;
					break;
				case 'b':
					icookie = arg(2);
					deal += 2;
					break;
				case 'c':
					ocookie = arg(2);
					deal += 2;
					break;
				case 'r':
					rawprint = true;
					deal += 1;
					break;
				case 'v':
					std::cout << version << std::endl;
					return 0;
				case 'h':
					for (const auto& s : netease::headers) {
						std::cout << s << '\n';
					}
					std::cout << "User-Agent: " << randomUserAgent() << '\n';
					return 0;
				}
			} else {
				break;
			}
		}
	} catch (...) {}
	curl_global_init(CURL_GLOBAL_ALL);
	struct {
		const char* cmd;
		std::function<void(void)> fun;
	} route[] = {
		"help", []() {
			std::cout << help;
		},
		"album", [&]() {
			std::cout << print(netease::album(atoi(arg(2))));
		},
		"artist", [&]() {
			std::cout << print(netease::artist(atoi(arg(2))));
		},
		"login.cellphone", [&]() {
			string pswd;
			close_echo();
			std::getline(std::cin, pswd);
			open_echo();
			std::cout << print(netease::login_cellphone(atoll(arg(2)), pswd));
		},
		"music.url", [&]() {
			int bitrate = 999000;
			try {
				bitrate = atoi(arg(3));
			} catch (...) {}
			std::cout << print(netease::music_url(atoi(arg(2)), bitrate));
		},
		"personal.fm", [&]() {
			std::cout << print(netease::personal_fm());
		},
		"search", [&]() {
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
			std::cout << print(netease::search(arg(2), st, limit, offset));
		},
		"song.detail", [&]() {
			std::cout << print(netease::song_detail(atoi(arg(2))));
		},
		"user.detail", [&]() {
			std::cout << print(netease::user_detail(atoi(arg(2))));
		},
		"user.playlist", [&]() {
			int limit = 30, offset = 0;
			try {
				limit = atoi(arg(3));
				offset = atoi(arg(4));
			} catch (...) {}
			std::cout << print(netease::user_playlist(atoi(arg(2)), limit, offset));
		}
	};
	try {
		const char* cmd = arg(1);
		bool found = false;
		for (const auto& p : route) {
			if (!strcmp(p.cmd, cmd)) {
				p.fun();
				found = true;
				break;
			}
		}
		if (!found) {
			throw "";
		}
	} catch (...) {
		std::cout << help;
		return 1;
	}
	curl_global_cleanup();
}