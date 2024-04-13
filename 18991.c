add_server_nonce(uint8_t *nonce)
{
    uint64_t ts;
    uint64_t tsn;
    uint32_t suffix;
    ts = dnscrypt_hrtime();
    // TODO? dnscrypt-wrapper does some logic with context->nonce_ts_last
    // unclear if we really need it, so skipping it for now.
    tsn = (ts << 10) | (randombytes_random() & 0x3ff);
#if (BYTE_ORDER == LITTLE_ENDIAN)
    tsn =
        (((uint64_t)htonl((uint32_t)tsn)) << 32) | htonl((uint32_t)(tsn >> 32));
#endif
    memcpy(nonce + crypto_box_HALF_NONCEBYTES, &tsn, 8);
    suffix = randombytes_random();
    memcpy(nonce + crypto_box_HALF_NONCEBYTES + 8, &suffix, 4);
}