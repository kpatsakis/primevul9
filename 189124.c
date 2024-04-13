ZRtp::ZRtp(uint8_t *myZid, ZrtpCallback *cb, std::string id, ZrtpConfigure* config, bool mitmm, bool sasSignSupport):
        callback(cb), dhContext(NULL), DHss(NULL), auxSecret(NULL), auxSecretLength(0), rs1Valid(false),
        rs2Valid(false), msgShaContext(NULL), hash(NULL), cipher(NULL), pubKey(NULL), sasType(NULL), authLength(NULL),
        multiStream(false), multiStreamAvailable(false), peerIsEnrolled(false), mitmSeen(false), pbxSecretTmp(NULL),
        enrollmentMode(false), configureAlgos(*config), zidRec(NULL) {

    enableMitmEnrollment = config->isTrustedMitM();
    paranoidMode = config->isParanoidMode();

    // setup the implicit hash function pointers and length
    hashLengthImpl = SHA256_DIGEST_LENGTH;
    hashFunctionImpl = sha256;
    hashListFunctionImpl = sha256;

    hmacFunctionImpl = hmac_sha256;
    hmacListFunctionImpl = hmac_sha256;

    memcpy(ownZid, myZid, ZID_SIZE);        // save the ZID

    /*
     * Generate H0 as a random number (256 bits, 32 bytes) and then
     * the hash chain, refer to chapter 9. Use the implicit hash function.
     */
    randomZRTP(H0, HASH_IMAGE_SIZE);
    sha256(H0, HASH_IMAGE_SIZE, H1);        // hash H0 and generate H1
    sha256(H1, HASH_IMAGE_SIZE, H2);        // H2
    sha256(H2, HASH_IMAGE_SIZE, H3);        // H3

    // configure all supported Hello packet versions
    zrtpHello_11.configureHello(&configureAlgos);
    zrtpHello_11.setH3(H3);                    // set H3 in Hello, included in helloHash
    zrtpHello_11.setZid(ownZid);
    zrtpHello_11.setVersion((uint8_t*)zrtpVersion_11);


    zrtpHello_12.configureHello(&configureAlgos);
    zrtpHello_12.setH3(H3);                 // set H3 in Hello, included in helloHash
    zrtpHello_12.setZid(ownZid);
    zrtpHello_12.setVersion((uint8_t*)zrtpVersion_12);

    if (mitmm) {                            // this session acts for a trusted MitM (PBX)
        zrtpHello_11.setMitmMode();
        zrtpHello_12.setMitmMode();
    }
    if (sasSignSupport) {                   // the application supports SAS signing
        zrtpHello_11.setSasSign();
        zrtpHello_12.setSasSign();
    }

    // Keep array in ascending order (greater index -> greater version)
    helloPackets[0].packet = &zrtpHello_11;
    helloPackets[0].version = zrtpHello_11.getVersionInt();
    setClientId(id, &helloPackets[0]);      // set id, compute HMAC and final helloHash

    helloPackets[1].packet = &zrtpHello_12;
    helloPackets[1].version = zrtpHello_12.getVersionInt();
    setClientId(id, &helloPackets[1]);      // set id, compute HMAC and final helloHash
 
    currentHelloPacket = helloPackets[SUPPORTED_ZRTP_VERSIONS-1].packet;  // start with highest supported version
    helloPackets[SUPPORTED_ZRTP_VERSIONS].packet = NULL;
    peerHelloVersion[0] = 0;

    stateEngine = new ZrtpStateClass(this);
}