void ZrtpQueue::srtpSecretsOff(EnableSecurity part) {
    if (part == ForSender) {
        removeOutQueueCryptoContext(NULL);
        removeOutQueueCryptoContextCtrl(NULL);
    }
    if (part == ForReceiver) {
        removeInQueueCryptoContext(NULL);
        removeInQueueCryptoContextCtrl(NULL);
    }
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->secureOff();
    }
}