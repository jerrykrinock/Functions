#import "SSLoginItemsAE.h"
#import "LoginItemsAE.h"
#import "SSApp/SSUtils.h"
#import "SSYLocalize/NSString+Localize.h"

BOOL SSLoginItemPathsWithAppName(NSString* appName, NSArray** pPaths)
{
	BOOL result ;
	
	NSMutableArray* loginItems = nil ;
	NSMutableArray** loginItemsPtr = &loginItems;
	
	NSInteger i;
	NSArray* paths = nil ;
	
	OSStatus err = LIAECopyLoginItems((CFArrayRef*)loginItemsPtr) ;
	
	if (err == noErr) {
		NSMutableArray* bucket = [[NSMutableArray alloc] init] ;
		
		for (i = 0; i < [loginItems count]; i++)
		{
			NSDictionary* itemDict = [loginItems objectAtIndex:i];
			NSString* itemPath = [[[itemDict objectForKey:@"URL"] absoluteString] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding] ;
			NSString* candidateAppName = [[itemPath lastPathComponent] stringByDeletingPathExtension] ;
			if ([candidateAppName isEqualToString:appName]) {
				[bucket addObject:itemPath] ;
			}
		}
		
		paths = [bucket copy] ;
		[bucket release] ;
		*pPaths = paths ;
		result = YES ;
	}
	else	{
		result = NO ;
		NSLog(@"Login Items AE failed in SSGetLoginItemsPathsWithAppName, err %li", (long)err) ;
	}
	
	[loginItems release];
	
	[paths autorelease] ;
	return result ;
}

enum SSResult SSTryIsLoginItem(NSString* filePath, BOOL* pAnswer, OSStatus* pErr)
{
	NSMutableArray* loginItems = nil ;
	NSMutableArray** loginItemsPtr = &loginItems;
	
	NSInteger i;
	
	if (!pErr) {
		OSStatus err ;
		pErr = &err ;
	}
	
	*pErr = LIAECopyLoginItems((CFArrayRef*)loginItemsPtr) ;
	
	
	NSInteger iOurIndex = NSNotFound ;
	if (*pErr == noErr)
	{
		NSString* executableName = [filePath lastPathComponent] ;
		for (i = 0; i < [loginItems count]; i++)
		{
			NSDictionary* itemDict = [loginItems objectAtIndex:i];
			NSString* itemPath = (NSString*)[[itemDict objectForKey:@"URL"] absoluteString] ;
			if ([itemPath rangeOfString:executableName].location != NSNotFound) {
				iOurIndex = i ;
			}
		}
	}
	[loginItems release];
	
	*pAnswer = (iOurIndex != NSNotFound) ;
	
	enum SSResult result = (*pErr == noErr) ? SSResultSucceeded : SSResultFailed ;
	
	return result ;
}


enum SSResult SSIsLoginItem(NSString* filePath, BOOL* pAnswer)
{
	OSStatus err ;
	
	enum SSResult result = SSResultFailed;
	
	if (filePath) {
		result = SSTryIsLoginItem(filePath, pAnswer, &err) ;
		
		if (result == SSResultFailed) {
			// Try again, and kill it this time
			NSUInteger oldPID = SSPIDOfRunningExecutableNamed(@"System Events") ;
			NSString* msg ;
			msg = [NSString stringWithFormat:@"Your System Events process (pid=%li) returned error l%i to our request for Login Items.  Since it is not working, we would like to kill and then restart your System Events process.  That usually fixes the problem.", (long)oldPID, (long)err] ;
			NSLog(msg) ;
			if (oldPID) {
				SSKillProcess(oldPID, YES) ;
				result = SSTryIsLoginItem(filePath, pAnswer, &err) ;
				if (result == SSResultFailed) {
					msg = [NSString stringWithFormat:@"Still didn't work.  err = %li.", (long)err] ;
					NSLog(msg) ;
				}
			}
		}
	}
	
	return result ;
}

enum SSResult SSTryAddDeleteUserLoginItem(NSString* filePath, BOOL hide, BOOL doAdd, OSStatus* pErr)
{
	enum SSResult result = SSResultNoAction ;
	
	NSMutableArray* loginItems = nil ;
	NSMutableArray** loginItemsPtr = &loginItems;
	//CFArrayRef* loginItemsPtr = (CFArrayRef*)&loginItems ;
	//NSLog(@"%x %x %x %x %x", *loginItems, loginItems, &loginItems, loginItemsPtr, *loginItemsPtr) ;
	
	NSInteger i;
	
	if (!pErr) {
		OSStatus err ;
		pErr = &err ;
	}
	
	*pErr = LIAECopyLoginItems((CFArrayRef*)loginItemsPtr) ;
	
	
	NSInteger iOurIndex = NSNotFound ;
	if (*pErr == noErr)
	{
		NSString* executableName = [filePath lastPathComponent] ;
		for (i = 0; i < [loginItems count]; i++)
		{
			NSDictionary* itemDict = [loginItems objectAtIndex:i];
			NSString* itemPath = (NSString*)[[itemDict objectForKey:@"URL"] absoluteString] ;
			if ([itemPath rangeOfString:executableName].location != NSNotFound) {
				iOurIndex = i ;
			}
		}
	}
	
	SSLog(5, "SSAddDeleteUserLoginItem: err=%i, iOurIndex=%i, doAdd=%i", *pErr, iOurIndex, doAdd) ;
	if ((*pErr==noErr) && (iOurIndex==NSNotFound) && doAdd)
	{
		// Bookwatchdog is not currently a login item but caller wants it in
		// So we add it
		SSLog(5, "Adding login item") ;
		NSURL* fileURL = [NSURL fileURLWithPath:filePath] ;
		*pErr = LIAEAddURLAtEnd((CFURLRef)fileURL, YES) ;
		result = SSResultAdded ;
	}
	else if ((*pErr==noErr) && (iOurIndex!=NSNotFound) && !doAdd)
	{
		// Bookwatchdog is currently a login item but caller wants it out
		// So we remove it
		SSLog(5, "Removing login item") ;
		*pErr = LIAERemove(iOurIndex) ;
		result = SSResultRemoved ;
	}
	
	if (*pErr != noErr)
	{
		result = SSResultFailed ;
		NSLog(@"Login Items AE failed, err %li", (long)(*pErr)) ;
	}
	
	[loginItems release];
	
	return result ;
}


enum SSResult SSAddDeleteUserLoginItem(NSString* filePath, BOOL hide, BOOL doAdd)
{
	OSStatus err ;
	
	enum SSResult result = SSResultFailed;
	
	if (filePath) {
		result = SSTryAddDeleteUserLoginItem(filePath, hide, doAdd, &err) ;
		
		if (result == SSResultFailed) {
			// Try again with dialogs shown to user
			uint32_t oldPID = SSPIDOfRunningExecutableNamed(@"System Events") ;
#define SQUAWK_SYSTEM_EVENTS 0
#if SQUAWK_SYSTEM_EVENTS
			NSInteger alertReturn ;
			alertReturn = NSRunInformationalAlertPanel(
													   nil,
													   [NSString stringWithFormat:@"Your System Events process (pid=%li) returned error %li to our request for Login Items.  Since it is not working, we would like to kill and then restart your System Events process.  That usually fixes the problem.", (long)oldPID, (long)err],
													   nil,
													   [NSString localize:@"cancel"],
													   nil) ;
			if (alertReturn == NSAlertDefaultReturn) {
				if (oldPID) {
					SSKillProcess(oldPID, YES) ;
					result = SSTryAddDeleteUserLoginItem(filePath, hide, doAdd, &err)	 ;			
					if (result == SSResultFailed)
					{
						NSRunInformationalAlertPanel(
													 [NSString localize:@"sorry"],
													 [NSString stringWithFormat:@"Still didn't work.  err = %li.", (long)err],
													 nil,
													 nil,
													 nil) ;
					} else {
						alertReturn = NSRunInformationalAlertPanel(
																   [NSString localize:@"itWorked"],
																   @"That fixed it",
																   nil,
																   nil,
																   nil) ;
					}
				}
			}
#else
			if (oldPID) {
				SSKillProcess(oldPID, YES) ;
				result = SSTryAddDeleteUserLoginItem(filePath, hide, doAdd, &err)	 ;			
				if (result == SSResultFailed) {
					result = SSResultNoAction ;
				}
			}
#endif
		}
	}
	
	return result ;
}
