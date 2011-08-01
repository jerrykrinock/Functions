#ifndef _LOGINITEMSAE_H
#define _LOGINITEMSAE_H

/////////////////////////////////////////////////////////////////

// System prototypes

#include <ApplicationServices/ApplicationServices.h>

/////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////

// Keys for the dictionary return by LIAECopyLoginItems.

#define kLIAEURL    CFSTR("URL")		// CFURL
#define kLIAEHidden CFSTR("Hidden")		// CFBoolean

extern OSStatus LIAECopyLoginItems(CFArrayRef *itemsPtr);
	// Returns an array of CFDictionaries, each one describing a 
	// login item.  Each dictionary has two elements, 
	// kLIAEURL and kLIAEHidden, which are 
	// documented above.
	// 
	// On input,    itemsPtr must not be NULL.
	// On input,   *itemsPtr must be NULL.
	// On success, *itemsPtr will be a pointer to a CFArray.
	// Or error,   *itemsPtr will be NULL.

extern OSStatus LIAEAddRefAtEnd(const FSRef *item, Boolean hideIt);
extern OSStatus LIAEAddURLAtEnd(CFURLRef item,     Boolean hideIt);
	// Add a new login item at the end of the list, using either 
	// an FSRef or a CFURL.  The hideIt parameter controls whether 
	// the item is hidden when it's launched.

extern OSStatus LIAERemove(CFIndex itemIndex);
	// Remove a login item.  itemIndex is an index into the array 
	// of login items as returned by LIAECopyLoginItems.

/////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif
