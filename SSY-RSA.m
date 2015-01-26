#import <Foundation/Foundation.h>
#import "SSY-RSA.h"

SSYVerifyRSAResult SSYVerifyRSA(
                                NSData* plainData,
                                NSData* signature,
                                NSData* publicKey,
                                NSError** error_p
                                ) {
    SSYVerifyRSAResult  result = SSYVerifyRSAResultInProgress ;
    CFErrorRef          errorCF = NULL ;
    
    CFArrayRef keyItems ;
    if (SecItemImport(
                      (__bridge CFDataRef)publicKey,
                      NULL,
                      NULL,
                      NULL,
                      0,
                      NULL,
                      NULL,
                      &keyItems
                      ) != noErr) {
        result = SSYVerifyRSAResultCouldNotImportPublicKey ;
    }
    
    SecKeyRef publicSecKeyRef = NULL ;

    if (result == SSYVerifyRSAResultInProgress) {
        publicSecKeyRef = (SecKeyRef)[(NSArray*)keyItems firstObject] ;
        if (publicSecKeyRef == NULL) {
            result = SSYVerifyRSAResultCouldNotExtractPublicKey ;
        }
    }
    
    SecTransformRef transform = NULL ;

    if (result == SSYVerifyRSAResultInProgress) {
        transform = SecVerifyTransformCreate(
                                             publicSecKeyRef,
                                             (__bridge CFDataRef)signature,
                                             &errorCF) ;
        if (transform == NULL) {
            result = SSYVerifyRSAResultCouldNotCreateTransform ;
        }
    }
    
    if (result == SSYVerifyRSAResultInProgress) {
        if (!SecTransformSetAttribute(
                                      transform,
                                      kSecDigestTypeAttribute,
                                      kSecDigestSHA1,
                                      &errorCF
                                      )) {
            result = SSYVerifyRSAResultCouldNotSetDigestType ;
        }
    }
    
    // Note: kSecInputIsAttributeName defaults to kSecInputIsPlainText, which
    // is what we want in this case.  So we skip setting that one.
    
    if (result == SSYVerifyRSAResultInProgress) {
        if (!SecTransformSetAttribute(
                                      transform,
                                      kSecTransformInputAttributeName,
                                      (__bridge CFDataRef) plainData,
                                      &errorCF
                                      )) {
            result = SSYVerifyRSAResultCouldNotSetInputType ;
        }
    }
    
    CFBooleanRef transformResult = NULL ;
    if (result == SSYVerifyRSAResultInProgress) {
        transformResult = SecTransformExecute(transform, &errorCF) ;
        if (transformResult == NULL) {
            result = SSYVerifyRSAResultCouldNotExecuteTransform ;
        }
    }
    
    if (result == SSYVerifyRSAResultInProgress) {
        if (CFGetTypeID(transformResult) != CFBooleanGetTypeID()) {
            result = SSYVerifyRSAResultTransformReturnedWrongType ;
        }
    }
    
    if (result == SSYVerifyRSAResultInProgress) {
        result = CFBooleanGetValue(transformResult)
        ? SSYVerifyRSAResultYippeeVerified
        : SSYVerifyRSAResultSorryDidNotVerify ;
    }
    
    if (errorCF && error_p) {
        *error_p = (NSError*)errorCF ;
#if NO_ARC
        [[*error_p retain] autorelease] ;
#endif
    }
    
#if 0
#if DEBUG
#warning Logging SSYRSAVerify()

    NSString* string ;

    string = [[NSString alloc] initWithData:plainData
                                   encoding:NSASCIIStringEncoding] ;
    NSLog(@"SSYRSAVerify: plainData:\n%@", string) ;
#if NO_ARC
    [string release] ;
#endif

    NSLog(@"SSYRSAVerify: signature (%ld bytes)\n%@",
          (long)[signature length],
          signature) ;
    
    string = [[NSString alloc] initWithData:publicKey
                                   encoding:NSASCIIStringEncoding] ;
    NSLog(@"SSYRSAVerify: publicKey:\n%@", string) ;
#if NO_ARC
    [string release] ;
#endif
    
    NSLog(@"SSYRSAVerify: result=%ld  Error: %@", result, (NSError*)errorCF) ;
    
#endif
#endif
    
    if (transformResult != NULL) {
        CFRelease(transformResult) ;
    }
    if (errorCF != NULL) {
        CFRelease(errorCF) ;
    }
    if (transform != NULL) {
        CFRelease(transform) ;
    }
    
    return result ;
}
