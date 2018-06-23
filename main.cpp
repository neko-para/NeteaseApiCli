#include "NeteaseApi.h"
#include <cstring>
#include <iostream>
#include <functional>

const char* help = 
R"(
NeteaseApi cmd params...
usage:
	~ help
	~ search text [type(song, album, singer, playlist, user)=song [limit(%d)=30 [offset(%d)=0]]]
)";

#define paramis(id, text) !strcmp(argv[id], #text)

int main(int argc, char* argv[]) {
	if (argc == 1) {
		return 1;
	} else {
		netease::init();
		if (paramis(1, help)) {
			std::cout << help;
		} else if (paramis(1, search)) {
			netease::SearchType st;
			int limit = 30, offset = 0;
			if (argc < 4) {
				return 1;
			} else {
				if (paramis(3, song)) {
					st = netease::ST_SONG;
				} else if (paramis(3, album)) {
					st = netease::ST_ALBUM;
				} else if (paramis(3, singer)) {
					st = netease::ST_SINGER;
				} else if (paramis(3, playlist)) {
					st = netease::ST_PLAYLIST;
				} else if (paramis(3, user)) {
					st = netease::ST_USER;
				} else {
					return 1;
				}
			}
			if (argc > 4) {
				limit = atoi(argv[4]);
				if (argc > 5) {
					offset = atoi(argv[5]);
					if (argc > 6) {
						netease::setcookie(argv[6]);
					}
				}
			}
			std::cout << netease::search(argv[2], st, limit, offset);
		}
		netease::exit();
	}
}