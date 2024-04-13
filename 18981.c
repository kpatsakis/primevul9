dnscrypt_server_curve(const dnsccert *cert,
                      uint8_t client_nonce[crypto_box_HALF_NONCEBYTES],
                      uint8_t nmkey[crypto_box_BEFORENMBYTES],
                      struct sldns_buffer* buffer,
                      uint8_t udp,
                      size_t max_udp_size)
{
    size_t dns_reply_len = sldns_buffer_limit(buffer);
    size_t max_len = dns_reply_len + DNSCRYPT_MAX_PADDING \
        + DNSCRYPT_REPLY_HEADER_SIZE;
    size_t max_reply_size = max_udp_size - 20U - 8U;
    uint8_t nonce[crypto_box_NONCEBYTES];
    uint8_t *boxed;
    uint8_t *const buf = sldns_buffer_begin(buffer);
    size_t len = sldns_buffer_limit(buffer);

    if(udp){
        if (max_len > max_reply_size)
            max_len = max_reply_size;
    }


    memcpy(nonce, client_nonce, crypto_box_HALF_NONCEBYTES);
    memcpy(nonce + crypto_box_HALF_NONCEBYTES, client_nonce,
           crypto_box_HALF_NONCEBYTES);

    boxed = buf + DNSCRYPT_REPLY_BOX_OFFSET;
    memmove(boxed + crypto_box_MACBYTES, buf, len);
    len = dnscrypt_pad(boxed + crypto_box_MACBYTES, len,
                       max_len - DNSCRYPT_REPLY_HEADER_SIZE, nonce,
                       cert->keypair->crypt_secretkey);
    sldns_buffer_set_at(buffer,
                        DNSCRYPT_REPLY_BOX_OFFSET - crypto_box_BOXZEROBYTES,
                        0, crypto_box_ZEROBYTES);

    // add server nonce extension
    add_server_nonce(nonce);

    if(cert->es_version[1] == 2) {
#ifdef USE_DNSCRYPT_XCHACHA20
        if (crypto_box_curve25519xchacha20poly1305_easy_afternm
            (boxed, boxed + crypto_box_MACBYTES, len, nonce, nmkey) != 0) {
            return -1;
        }
#else
        return -1;
#endif
    } else {
        if (crypto_box_easy_afternm
            (boxed, boxed + crypto_box_MACBYTES, len, nonce, nmkey) != 0) {
            return -1;
        }
    }

    sldns_buffer_write_at(buffer,
                          0,
                          DNSCRYPT_MAGIC_RESPONSE,
                          DNSCRYPT_MAGIC_HEADER_LEN);
    sldns_buffer_write_at(buffer,
                          DNSCRYPT_MAGIC_HEADER_LEN,
                          nonce,
                          crypto_box_NONCEBYTES);
    sldns_buffer_set_limit(buffer, len + DNSCRYPT_REPLY_HEADER_SIZE);
    return 0;
}