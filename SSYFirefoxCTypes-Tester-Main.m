#import "SSYFirefoxCTypes.h"

/* 
 Mozilla's Xulrunner dylibs are apparently compiled with the magic  @executable_path
 in their INSTALL_PATH.  A Firefox Extension linking to the Xulrunner dylibs works when
 run in Firefox because the Xulrunner dylibs are packaged in
 /Applications/Firefox.app/Contents/MacOS/, alongside the executable,
 ("firefox-bin" or "firefox").
 
 So, because this tool is linked to these Xulrunner dylibs, you need these Xulrunner
 dylibs to be available at the same relative path as they are in the Firefox package,
 which is alongside the executable produced.  The dylibs needed are:
 
 libmozalloc.dylib
 libmozsqlite3.dylib
 libnspr4.dylib
 libnss3.dylib
 libnssutil3.dylib
 libplc4.dylib
 libplds4.dylib
 libsmime3.dylib
 libssl3.dylib
 libxpcom.dylib
 
 You also need this executable.  I don't know why:
 XUL
 
 The above files are all available in any xulrunner SDK.
 Some are in the bin/ subdirectory, some are in lib/.
 */
int main(int argc, const char *argv[]) {
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init] ;

	int32_t sum = SSYC_demo_int(22,33) ;
	NSLog(@"dylib returned sum %ld", (long)sum) ;
	
	char* s = "Hello Firefox" ;
	const char* uppedS = SSYC_demo_string(s) ;
	NSLog(@"dylib returned upped '%s' to '%s'", s, uppedS) ;
	
	SSYC_startServer("My-Server") ;
	
	[pool drain] ;
	
	return 0 ;
}