bool ZRtp::verifyH2(ZrtpPacketCommit *commit) {
    uint8_t tmpH3[IMPL_MAX_DIGEST_LENGTH];

    sha256(commit->getH2(), HASH_IMAGE_SIZE, tmpH3);
    if (memcmp(tmpH3, peerH3, HASH_IMAGE_SIZE) != 0) {
        return false;
    }
    return true;
}