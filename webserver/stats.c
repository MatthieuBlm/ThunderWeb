#include "socket.h"
static web_stats *stats;

int init_stats ( void ){
	stats = mmap(NULL, 500, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	stats-> served_connections = 0;
	stats-> served_requests = 0;
	stats-> ok_200 = 0;
	stats-> ko_400 = 0;
	stats-> ko_403 = 0;
	stats-> ko_404 = 0;
	return 0;
}

web_stats * get_stats ( void ){
	return stats;
}
