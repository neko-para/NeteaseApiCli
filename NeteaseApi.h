#ifndef _NEATEASE_API_H_
#define _NEATEASE_API_H_

#include <string>
using std::string;

namespace netease {

	void init();
	void exit();

	void setcookie(const string& data);
	string getcookie();

	enum SearchType {
		ST_SONG = 1,
		ST_ALBUM = 10,
		ST_ARTIST = 100,
		ST_PLAYLIST = 1000,
		ST_USER = 1002
	};

	string album(int id);
	string artist(int id);
	string search(const string& keyword, SearchType type, int limit = 30, int offset = 0);
	string song_detail(int id);
	string user_detail(int id);
	string user_playlist(int id, int limit = 30, int offset = 0);

}

#endif