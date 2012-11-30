#ifndef SSY_ASL_h
#define SSY_ASL_h

// We include asl.h here so that users including this file will have the ASL
// level preprocessor definitions such as ASL_LEVEL_EMERG defined.
#include <asl.h>

/*!
 @brief    A wrapper around the Apple System Log (ASL) functions
 
 @details  I wrote this wrapper because I was unable to get output from the
 regular ASL (asl_) functions when invoked from a Privileged Helper Tool
 which was launched by launchd, and also because I think Apple's ASL functions
 have too many parameters for my purposes and give me a headache.
 
 Entries logged by the functions in this wrapper are guaranteed to be written to
 ~/Library/Logs/<__prograname>.log if your program is not running as root, or
 /Library/Logs/<__prograname>.log if your program is running as root.  They
 are supposedly written to stderr too, but as noted above, that may not be
 available.
 
 This wrapper creates some global variables for ASL.  A global aslclient and
 a file descriptor to the <__prograname>.log file are set when you make your
 first log entry during program execution, by calling, for example,
 ssyasl_log().
 */

/*!
 @brief    Sets the current SSY_ASL priority level to a given level
 
 @details  Prior to calling this function, the SSY_ASL priority level is 
 defaulted to ASL_LEVEL_NOTICE.  It is OK to call this function before
 your first log entry, which means before the log file is opened.  It is
 simply remembered in a global int variable.
 */
void ssyasl_set_filter(int level) ;

/*!
 @brief    Logs a message to ~/Library/Logs/<__progname>.log, if its given
 level is equal to or severe than the current SSL_ASL priority level
 */
void ssyasl_log(char* progname, int level, char* format, ...) ;

/*!
 @brief    Invokes ssy_log with level ASL_LEVEL_NOTICE
 */
void ssyasl_note(char* progname, char* format, ...) ;


/*!
 @brief    Closes the global aslclient and the file descriptor to the log file,
 if these are currently open
 
 @details  This function is provided for sticklers about tidy programming.
 You can call it any time, actually.  They will be reopened if you make more
 log entries later.
 
 Since the system closes open file descriptors when a program exits, it is
 not really necessary to ever use this function.
 */
void ssyasl_close() ;

#endif
