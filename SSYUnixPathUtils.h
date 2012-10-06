#define SSY_UNIX_PATH_UTILS_MAX_PATH_CHARS 2047

#include <sys/types.h>
#import <stdint.h>

/*!
 @brief    Given a unix path, removes the last path component
 of the given path and returns the resulting "parent path" by
 reference.
 
 @details  The end of the parent is assumed to be marked by the
 last slash character ('/') in the given path.
 
 @param    parent  The string buffer into which the parent
 path will be copied.  The returned string is NULL-terminated
 and includes the trailing slash.  The caller is responsible
 to make sure that the size of parent is at least that of
 path, to avoid buffer overruns.
 @result   If a parent was found, returns 0.  If a parent
 cannot be found in the given path (because it does not
 contain a slash), returns -1.
 */
int UnixPathParent(char* parent, char* path) ;

/*!
 @brief    Returns by reference the unix user ID (uid)
 and group ID (gid) who "own" the *parent* of a given path.
 
 @details  Uses the stat(3) function to determine the
 owners.
 
 @param    path  The path whose parent's owner is
 desired.  Must be less than SSY_UNIX_PATH_UTILS_MAX_PATH_CHARS
 characters.  The item implied by path need not really
 exist in the filesystem; only its parent, which you get
 from  truncating the string after the last slash, must
 actually exist. 
 @param    uid_p  If successful, will, upon return, point
 to the user ID (uid) of the parent of the given path
 @param    gid_p  If successful, will, upon return, point
 to the group ID (gid) of the parent of the given path
 @result   If successful, returns 0. If stat(3) failed for
 the given path, returns -1 and sets errno.  If the given
 path's length exceeds MAX_PATH_CHARS, returns -2.  If the
 given path does not have have a parent (for example,
 if it does not contain a slash), returns -3.
 */
int UnixOwnerIDs(char* path, uid_t* uid_p, gid_t* gid_p) ;
