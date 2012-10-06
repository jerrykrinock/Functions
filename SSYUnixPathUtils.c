#include "SSYUnixPathUtils.h"
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

int UnixPathParent(char* parent, char* path) {
	int len = (int)strlen(path) ;
	int i ;
	short status = -1 ;
	for (i=(len-2); i>=0; i--) {
		if (status != 0) {
			if (path[i] == '/') {
				parent[i+1] = 0 ;
				parent[i] = '/' ;
				status = 0 ;
			}
		}
		else  {
			parent[i] = path[i] ;
		}
	}
	return status ;
}

int UnixOwnerIDs(char* path, uid_t* uid_p, gid_t* gid_p) {
	if (strlen(path) > SSY_UNIX_PATH_UTILS_MAX_PATH_CHARS) {
		return -2 ;
	}
	if (strlen(path) < 2) {
		return -3 ;
	}
	
	char parent[SSY_UNIX_PATH_UTILS_MAX_PATH_CHARS+1] ;
	int result = UnixPathParent(parent, path) ;
	if (result == -1) {
		return -3 ;
	}
	struct stat parent_stat ;
	result = stat(parent, &parent_stat) ;
	if (result == -1) {
		return -1 ;
	}
	
	if (uid_p) {
		*uid_p = parent_stat.st_uid ;
	}
	if (gid_p) {
		*gid_p = parent_stat.st_gid ;
	}
	
	return 0 ;
}

