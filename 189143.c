bool ZrtpQueue::srtpSecretsReady(SrtpSecret_t* secrets, EnableSecurity part)
{
    CryptoContext* recvCryptoContext;
    CryptoContext* senderCryptoContext;
    CryptoContextCtrl* recvCryptoContextCtrl;
    CryptoContextCtrl* senderCryptoContextCtrl;

    int cipher;
    int authn;
    int authKeyLen;

    if (secrets->authAlgorithm == Sha1) {
        authn = SrtpAuthenticationSha1Hmac;
        authKeyLen = 20;
    }

    if (secrets->authAlgorithm == Skein) {
        authn = SrtpAuthenticationSkeinHmac;
        authKeyLen = 32;
    }

    if (secrets->symEncAlgorithm == Aes)
        cipher = SrtpEncryptionAESCM;

    if (secrets->symEncAlgorithm == TwoFish)
        cipher = SrtpEncryptionTWOCM;

    if (part == ForSender) {
        // To encrypt packets: intiator uses initiator keys,
        // responder uses responder keys
        // Create a "half baked" crypto context first and store it. This is
        // the main crypto context for the sending part of the connection.
        if (secrets->role == Initiator) {
            senderCryptoContext = new CryptoContext(
                    0,
                    0,
                    0L,                                      // keyderivation << 48,
                    cipher,                                  // encryption algo
                    authn,                                   // authtentication algo
                    (unsigned char*)secrets->keyInitiator,   // Master Key
                    secrets->initKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltInitiator,  // Master Salt
                    secrets->initSaltLen / 8,                // Master Salt length
                    secrets->initKeyLen / 8,                 // encryption keyl
                    authKeyLen,                              // authentication key len
                    secrets->initSaltLen / 8,                // session salt len
                    secrets->srtpAuthTagLen / 8);            // authentication tag lenA
            senderCryptoContextCtrl = new CryptoContextCtrl(0,
                  cipher,                                    // encryption algo
                  authn,                                     // authtication algo
                  (unsigned char*)secrets->keyInitiator,     // Master Key
                  secrets->initKeyLen / 8,                   // Master Key length
                  (unsigned char*)secrets->saltInitiator,    // Master Salt
                  secrets->initSaltLen / 8,                  // Master Salt length
                  secrets->initKeyLen / 8,                   // encryption keyl
                  authKeyLen,                                // authentication key len
                  secrets->initSaltLen / 8,                  // session salt len
                  secrets->srtpAuthTagLen / 8);              // authentication tag len
        }
        else {
            senderCryptoContext = new CryptoContext(
                    0,
                    0,
                    0L,                                      // keyderivation << 48,
                    cipher,                                  // encryption algo
                    authn,                                   // authtentication algo
                    (unsigned char*)secrets->keyResponder,   // Master Key
                    secrets->respKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltResponder,  // Master Salt
                    secrets->respSaltLen / 8,                // Master Salt length
                    secrets->respKeyLen / 8,                 // encryption keyl
                    authKeyLen,                              // authentication key len
                    secrets->respSaltLen / 8,                // session salt len
                    secrets->srtpAuthTagLen / 8);            // authentication tag len
            senderCryptoContextCtrl = new CryptoContextCtrl(0,
                  cipher,                                    // encryption algo
                  authn,                                     // authtication algo
                  (unsigned char*)secrets->keyResponder,     // Master Key
                  secrets->respKeyLen / 8,                   // Master Key length
                  (unsigned char*)secrets->saltResponder,    // Master Salt
                  secrets->respSaltLen / 8,                  // Master Salt length
                  secrets->respKeyLen / 8,                   // encryption keyl
                  authKeyLen,                                // authentication key len
                  secrets->respSaltLen / 8,                  // session salt len
                  secrets->srtpAuthTagLen / 8);              // authentication tag len
        }
        if (senderCryptoContext == NULL) {
            return false;
        }
        // Insert the Crypto templates (SSRC == 0) into the queue. When we send
        // the first RTP or RTCP packet the real crypto context will be created.
        // Refer to putData(), sendImmediate() in ccrtp's outqueue.cpp and
        // takeinControlPacket() in ccrtp's control.cpp.
        //
         setOutQueueCryptoContext(senderCryptoContext);
         setOutQueueCryptoContextCtrl(senderCryptoContextCtrl);
    }
    if (part == ForReceiver) {
        // To decrypt packets: intiator uses responder keys,
        // responder initiator keys
        // See comment above.
        if (secrets->role == Initiator) {
            recvCryptoContext = new CryptoContext(
                    0,
                    0,
                    0L,                                      // keyderivation << 48,
                    cipher,                                  // encryption algo
                    authn,                                   // authtentication algo
                    (unsigned char*)secrets->keyResponder,   // Master Key
                    secrets->respKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltResponder,  // Master Salt
                    secrets->respSaltLen / 8,                // Master Salt length
                    secrets->respKeyLen / 8,                 // encryption keyl
                    authKeyLen,                              // authentication key len
                    secrets->respSaltLen / 8,                // session salt len
                    secrets->srtpAuthTagLen / 8);            // authentication tag len
            recvCryptoContextCtrl = new CryptoContextCtrl(0,
                  cipher,                                    // encryption algo
                  authn,                                     // authtication algo
                  (unsigned char*)secrets->keyResponder,     // Master Key
                  secrets->respKeyLen / 8,                   // Master Key length
                  (unsigned char*)secrets->saltResponder,    // Master Salt
                  secrets->respSaltLen / 8,                  // Master Salt length
                  secrets->respKeyLen / 8,                   // encryption keyl
                  authKeyLen,                                // authentication key len
                  secrets->respSaltLen / 8,                  // session salt len
                  secrets->srtpAuthTagLen / 8);              // authentication tag len

        }
        else {
            recvCryptoContext = new CryptoContext(
                    0,
                    0,
                    0L,                                      // keyderivation << 48,
                    cipher,                                  // encryption algo
                    authn,                                   // authtentication algo
                    (unsigned char*)secrets->keyInitiator,   // Master Key
                    secrets->initKeyLen / 8,                 // Master Key length
                    (unsigned char*)secrets->saltInitiator,  // Master Salt
                    secrets->initSaltLen / 8,                // Master Salt length
                    secrets->initKeyLen / 8,                 // encryption keyl
                    authKeyLen,                              // authentication key len
                    secrets->initSaltLen / 8,                // session salt len
                    secrets->srtpAuthTagLen / 8);            // authentication tag len
            recvCryptoContextCtrl = new CryptoContextCtrl(0,
                  cipher,                                    // encryption algo
                  authn,                                     // authtication algo
                  (unsigned char*)secrets->keyInitiator,     // Master Key
                  secrets->initKeyLen / 8,                   // Master Key length
                  (unsigned char*)secrets->saltInitiator,    // Master Salt
                  secrets->initSaltLen / 8,                  // Master Salt length
                  secrets->initKeyLen / 8,                   // encryption keyl
                  authKeyLen,                                // authentication key len
                  secrets->initSaltLen / 8,                  // session salt len
                  secrets->srtpAuthTagLen / 8);              // authentication tag len
        }
        if (recvCryptoContext == NULL) {
            return false;
        }
        // Insert the Crypto templates (SSRC == 0) into the queue. When we receive
        // the first RTP or RTCP packet the real crypto context will be created.
        // Refer to rtpDataPacket() above and takeinControlPacket in ccrtp's control.cpp.
        //
        setInQueueCryptoContext(recvCryptoContext);
        setInQueueCryptoContextCtrl(recvCryptoContextCtrl);
    }
    return true;
}