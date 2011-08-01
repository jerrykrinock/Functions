
BOOL SSLoginItemPathsWithAppName(NSString* appName, NSArray** pPaths) ;
// appName is the app name being searched for; do NOT include the .app extension
// Returns NO if LoginItemsAE failed, normally returns YES
// pPaths will be an autoreleased array of full pathnames
// *pErr is the OSStatus returned by LoginItemsAE

enum SSResult SSIsLoginItem(NSString* filePath, BOOL* pAnswer) ;
// Using old CFPreferences technique, filePath must end in ".app".
// Now that we are using LoginItemsAE, maybe this is no longer necessary
// Possible return values: SSResultSucceeded, SSResultFailed

enum SSResult SSAddDeleteUserLoginItem(NSString* filePath, BOOL hide, BOOL doAdd) ;
// Using old CFPreferences technique, filePath must end in ".app".
// Now that we are using LoginItemsAE, maybe this is no longer necessary
// If doAdd=1, will add item.  If doAdd=0, will delete item.
// if doAdd=1, full path is recommended.  If doAdd=0, may pass executable name as fullPath
// Shows alert to user and retries if fail
// Possible return values: SSResultNoAction, SSResultAdded, SSResultRemoved, SSResultFailed

enum SSResult SSTryAddDeleteUserLoginItem(NSString* filePath, BOOL hide, BOOL doAdd, OSStatus* pErr) ;
// Same as above, but does not show alert to user, just fails
// pErr may be NULL if not interested

