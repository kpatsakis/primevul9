uint64_t DecimalQuantity::getPositionFingerprint() const {
    uint64_t fingerprint = 0;
    fingerprint ^= lOptPos;
    fingerprint ^= (lReqPos << 16);
    fingerprint ^= (static_cast<uint64_t>(rReqPos) << 32);
    fingerprint ^= (static_cast<uint64_t>(rOptPos) << 48);
    return fingerprint;
}