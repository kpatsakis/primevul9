bool ZrtpQueue::checkSASSignature(uint8_t* sasHash) {
    if (zrtpUserCallback != NULL) {
        return zrtpUserCallback->checkSASSignature(sasHash);
    }
    return false;
}