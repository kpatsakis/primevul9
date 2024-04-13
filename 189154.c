void ZRtp::setAuxSecret(uint8_t* data, int32_t length) {
    if (length > 0) {
        auxSecret = new uint8_t[length];
        auxSecretLength = length;
        memcpy(auxSecret, data, length);
    }
}