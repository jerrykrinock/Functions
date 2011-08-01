#include <unistd.h>  // for uid_t

/*!
 @brief    Wraps chown(2) with fts(3) so that it operates recursively,
 like chown(8) with -R option.
 
 @details  Parameters are same as chown(2).
 @result   Returns 0 if successful.
 
 Returns -1 if chown fails for the given path or any descendant,
 which usually happens if you don't have the elevated privileges
 required to chown things.  To provide the reason for the failure,
 chown(2) will set errno.
 
 Returns -2 if fts_open() could not be initialized.  I don't know how
 -2 could ever happen though, because fts_open()
 still returns 0 even if you give it a nonexistent path.
 */
int chown_recursive(char * const path, uid_t uid, uid_t gid) ;
