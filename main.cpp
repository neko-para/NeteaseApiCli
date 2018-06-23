#include "NeteaseApi.h"
#include <iostream>

int main() {
	netease::init();
	std::cout << netease::search("nekosulvr", netease::ST_USER);
	netease::exit();
}