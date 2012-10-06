#include "RecursiveChown.h"
#include <fts.h>
#include <stdio.h>


int16_t chown_recursive(char * const path, uid_t uid, uid_t gid) {
	// See man fts(3) for these.  Modify these to do what you want:
	int fts_options =  FTS_PHYSICAL | FTS_NOCHDIR | FTS_XDEV | FTS_SEEDOT ;
	
	// fts_open requires a null-terminated array of paths.
	// We only have one path.
	char* paths[2] ;
	paths[0] = path ;
	paths[1] = NULL ;
	
	FTS* ftsp = fts_open(paths, fts_options, NULL) ;
	if (ftsp == NULL) {
		return -2 ;
	}
	
	FTSENT* ftsPointer = 0 ;
	while ((ftsPointer = fts_read(ftsp)) != NULL) {
		/*
		 This will execute once for each item in the tree.
		 According to the man page fts(3):
		 "directories are visited two distinguishable times; in pre-order
		 (before any of their descendants are visited) and in post-order
		 (after all of their descendants have been visited)"
		 But if I replace chown() below with a printf(), it only logs once for
		 each directory, in pre-order.  Example:
		 Doing /Users/jk/Desktop/OW5/.
		 Doing /Users/jk/Desktop/OW5/..
		 Doing /Users/jk/Desktop/OW5/BkmxTemp/.
		 Doing /Users/jk/Desktop/OW5/BkmxTemp/..
		 Doing /Users/jk/Desktop/OW5/BkmxTemp/placesNo1Dump.txt
		 Doing /Users/jk/Desktop/OW5/BkmxTemp/placesOk1Dump.txt
		 Apparently a "visit" does not mean "returned by ftsPointer->fts_info"
		 */
		int result ;
		switch (ftsPointer->fts_info) { 
			// List here the file types you want to be chown-ed.
			// We want all types.
			case FTS_F:     // regular file
			case FTS_D:     // directory
			case FTS_DOT:   // system dot file, i.e. '.' or '..' 
			case FTS_SL:    // symbolic link
				result = chown(ftsPointer->fts_path, uid, gid) ;
				if (result != 0) {
					// chown only returns either 0 or -1.
					// Must be -1 here.
					return result ;
				}
				break ;
				
			default:
				printf("Internal Error 524-0192 unknown type %d\n", ftsPointer->fts_info) ;
				break; 
		}
	}
	
	fts_close(ftsp) ;
	
	return 0 ;
	
}