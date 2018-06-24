#include "NeteaseApi.h"
#include <cstring>
#include <iostream>
#include <functional>

const char* help = 
R"(
NeteaseApi cmd params...
usage:
	~ help
	~ album id(%d)
	~ artist id(%d)
	~ search text [type(song, album, artist, playlist, user)=song [limit(%d)=30 [offset(%d)=0]]]
)";

#define paramis(id, text) !strcmp(arg(id), #text)

int main(int argc, char* argv[]) {
	auto arg = [=](int i) -> char* {
		if (argc <= i) {
			throw "";
		} else {
			return argv[i];
		}
	};
	try {
		netease::init();
		if (paramis(1, help)) {
			std::cout << help;
		} else if (paramis(1, album)) {
			std::cout << netease::album(atoi(arg(2)));
		} else if (paramis(1, artist)) {
			std::cout << netease::artist(atoi(arg(2)));
		} else if (paramis(1, search)) {
			netease::SearchType st;
			int limit = 30, offset = 0;
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
			} else {
				return 1;
			}
			try {
				limit = atoi(arg(4));
				offset = atoi(arg(5));
				netease::setcookie(arg(6));
			} catch (...) {

			}
			std::cout << netease::search(arg(2), st, limit, offset);
		}
		netease::exit();
	} catch (...) {
		std::cout << help;
		return 1;
	}
}