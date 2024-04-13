bool ZRtp::srtpSecretsReady(EnableSecurity part) {

    SrtpSecret_t sec;

    sec.symEncAlgorithm = cipher->getAlgoId();

    sec.keyInitiator = srtpKeyI;
    sec.initKeyLen = cipher->getKeylen() * 8;
    sec.saltInitiator = srtpSaltI;
    sec.initSaltLen = 112;

    sec.keyResponder = srtpKeyR;
    sec.respKeyLen = cipher->getKeylen() * 8;
    sec.saltResponder = srtpSaltR;
    sec.respSaltLen = 112;

    sec.authAlgorithm = authLength->getAlgoId();
    sec.srtpAuthTagLen = authLength->getKeylen();

    sec.sas = SAS;
    sec.role = myRole;

    bool rc = callback->srtpSecretsReady(&sec, part);

    // The call state engine calls ForSender always after ForReceiver.
    if (part == ForSender) {
        std::string cs(cipher->getReadable());
        if (!multiStream) {
            cs.append("/").append(pubKey->getName());
            if (mitmSeen)
                cs.append("/EndAtMitM");
            callback->srtpSecretsOn(cs, SAS, zidRec->isSasVerified());
        }
        else {
            std::string cs1("");
            if (mitmSeen)
                cs.append("/EndAtMitM");
            callback->srtpSecretsOn(cs, cs1, true);
        }
    }
    return rc;
}