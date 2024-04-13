void ZRtp::KDF(uint8_t* key, uint32_t keyLength, uint8_t* label, int32_t labelLength,
               uint8_t* context, int32_t contextLength, int32_t L, uint8_t* output) {

    unsigned char* data[6];
    uint32_t length[6];
    uint32_t pos = 0;                  // index into the array
    uint32_t maclen = 0;

    // Very first element is a fixed counter, big endian
    uint32_t counter = 1;
    counter = zrtpHtonl(counter);
    data[pos] = (unsigned char*)&counter;
    length[pos++] = sizeof(uint32_t);

    // Next element is the label, null terminated, labelLength includes null byte.
    data[pos] = label;
    length[pos++] = labelLength;

    // Next is the KDF context
    data[pos] = context;
    length[pos++] = contextLength;

    // last element is HMAC length in bits, big endian
    uint32_t len = zrtpHtonl(L);
    data[pos] = (unsigned char*)&len;
    length[pos++] = sizeof(uint32_t);

    data[pos] = NULL;

    // Use negotiated hash.
    hmacListFunction(key, keyLength, data, length, output, &maclen);
}