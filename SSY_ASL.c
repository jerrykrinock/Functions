#include "SSY_ASL.h"
#include <libgen.h>
#include <CoreFoundation/CoreFoundation.h>

int ssyasl_gFileDescriptor = 0 ;
aslclient ssyasl_gAslClient = NULL ;
int ssyasl_gFilter = ASL_LEVEL_EMERG ;

/*!
 @brief    Handy function for concatenating two C strings
 
 @details  The returned result has been mallocced and the caller should free
 it when it is no longer needed.
 */
static char* create_cat_string(char* s1, char* s2) {
    char* result = malloc(snprintf(NULL, 0, "%s%s", s1, s2) + 1) ;
    sprintf(result, "%s%s", s1, s2) ;
    return result ;
}

char* ssyasl_create_log_path(char* progname) {
    char* home = getenv("HOME") ;
    if (home == NULL) {
        // We must be running as root
        home = "" ;
    }
    char* path1 = create_cat_string(home, "/Library/Logs/") ;
    
    /* At first, I used the BSD global variable __progname here, cheating
     by declaring it as extern because it is not declared in the SDK.  Although
     __programe is defined in the macOS 10.8 runtime, it is not defined in
     the 10.6 runtime.  Without __progname, we have to pass the stupid
     parameter progname to this function. */
    char* path2 = create_cat_string(path1, progname) ;
    free(path1) ;
    char* path = create_cat_string(path2, ".log") ;
    free(path2) ;
    
    printf("path: %s\n", path) ;
    return path ;
}

/*!
 @details  This function is necessary because, for some strange reason,
 the Apple System Log system does not filter entries written to log
 files using the parameter set in asl_set_filter().  What were they thinking?
 */
void ssyasl_set_filter(int level) {
    ssyasl_gFilter = level ;
}

void ssyasl_init(char* progname) {
    ssyasl_gAslClient = asl_open(NULL, NULL, ASL_OPT_STDERR) ;

    char* path = ssyasl_create_log_path(progname) ;
    ssyasl_gFileDescriptor = open(path, FWRITE | O_CREAT | O_APPEND,
                                  S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) ;
    free(path) ;
    asl_add_log_file(ssyasl_gAslClient, ssyasl_gFileDescriptor) ;
    
    // Without the following, you sometimes get two sysbeeps whenever asl_log is
    // invoked with level ASL_LEVEL_EMERG.  Makes no sense to me.
    asl_set_filter(ssyasl_gAslClient, ASL_LEVEL_EMERG) ;

    ssyasl_set_filter(ASL_LEVEL_NOTICE) ;
}

void ssyasl_log(char* progname, int level, char* format, ...) {
	if (ssyasl_gAslClient == NULL) {
        ssyasl_init(progname);
    }
    
    if (level <= ssyasl_gFilter) {
        va_list argp;
        va_start(argp, format);
        asl_vlog(ssyasl_gAslClient, NULL, level, format, argp) ;
        va_end(argp);
    }
}

void ssyasl_note(char* progname, char* format, ...) {
	if (ssyasl_gAslClient == NULL) {
        ssyasl_init(progname);
    }
    
    int level = ASL_LEVEL_NOTICE ;
    if (level <= ssyasl_gFilter) {
        va_list argp;
        va_start(argp, format);
        asl_vlog(ssyasl_gAslClient, NULL, level, format, argp) ;
        va_end(argp);
    }
}

void ssyasl_close(void) {
    if (ssyasl_gAslClient != NULL) {
        asl_close(ssyasl_gAslClient) ;
        ssyasl_gAslClient = NULL ;
    }

    if (ssyasl_gFileDescriptor != 0) {
        close(ssyasl_gFileDescriptor) ;
        ssyasl_gFileDescriptor = 0 ;
    }
}

