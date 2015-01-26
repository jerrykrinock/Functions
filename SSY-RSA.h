#ifndef SSYRSA_h
#define SSYRSA_h

typedef enum : NSUInteger {
    SSYVerifyRSAResultInProgress = 0,
    SSYVerifyRSAResultYippeeVerified = 1,
    SSYVerifyRSAResultSorryDidNotVerify = 2,
    SSYVerifyRSAResultTransformReturnedWrongType,
    SSYVerifyRSAResultCouldNotExecuteTransform,
    SSYVerifyRSAResultCouldNotSetInputType,
    SSYVerifyRSAResultCouldNotSetDigestType,
    SSYVerifyRSAResultCouldNotCreateTransform,
    SSYVerifyRSAResultCouldNotExtractPublicKey,
    SSYVerifyRSAResultCouldNotImportPublicKey,
} SSYVerifyRSAResult ;

/*!
 @brief    Verifies an RSA / ASN.1 / SHA-1 signature
 
 @details  Determines whether or not a given signature was produced by
 producing a SHA-1 digest of a given plain-text data, prepending a ASN.1 header
 to it per Section 9.2 of RFC 3447, and signing the result with an RSA private
 key whose public key is also given.
 
 @param    plainData  Typically you derive this from a string using
 -[NSString dataUsingEncoding:].
 
 @param    publicKey  Typically you derive this from a string of the form
 -----BEGIN PUBLIC KEY-----
 MIIBIjANB...
 ...AQAB
 -----END PUBLIC KEY-----
 using -[NSString dataUsingEncoding:].
 
 This function uses Apple's Security Transform API, and therefore requires OS X
 10.7 or later.  Does not work in iOS.  Does not use openssl.
 
 This code is based on Apple's CryptoCompatibility sample code, which may be
 helpful in forking this code for similar purposes.
 */
SSYVerifyRSAResult SSYVerifyRSA(
                                NSData* plainData,
                                NSData* signature,
                                NSData* publicKey,
                                NSError** error_p
                                ) ;

#endif
