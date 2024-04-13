static int ssl_write_ticket( ssl_context *ssl, size_t *tlen )
{
    int ret;
    unsigned char * const start = ssl->out_msg + 10;
    unsigned char *p = start;
    unsigned char *state;
    unsigned char iv[16];
    size_t clear_len, enc_len, pad_len, i;

    *tlen = 0;

    if( ssl->ticket_keys == NULL )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    /* Write key name */
    memcpy( p, ssl->ticket_keys->key_name, 16 );
    p += 16;

    /* Generate and write IV (with a copy for aes_crypt) */
    if( ( ret = ssl->f_rng( ssl->p_rng, p, 16 ) ) != 0 )
        return( ret );
    memcpy( iv, p, 16 );
    p += 16;

    /*
     * Dump session state
     *
     * After the session state itself, we still need room for 16 bytes of
     * padding and 32 bytes of MAC, so there's only so much room left
     */
    state = p + 2;
    if( ssl_save_session( ssl->session_negotiate, state,
                          SSL_MAX_CONTENT_LEN - ( state - ssl->out_ctr ) - 48,
                          &clear_len ) != 0 )
    {
        return( POLARSSL_ERR_SSL_CERTIFICATE_TOO_LARGE );
    }
    SSL_DEBUG_BUF( 3, "session ticket cleartext", state, clear_len );

    /* Apply PKCS padding */
    pad_len = 16 - clear_len % 16;
    enc_len = clear_len + pad_len;
    for( i = clear_len; i < enc_len; i++ )
        state[i] = (unsigned char) pad_len;

    /* Encrypt */
    if( ( ret = aes_crypt_cbc( &ssl->ticket_keys->enc, AES_ENCRYPT,
                               enc_len, iv, state, state ) ) != 0 )
    {
        return( ret );
    }

    /* Write length */
    *p++ = (unsigned char)( ( enc_len >> 8 ) & 0xFF );
    *p++ = (unsigned char)( ( enc_len      ) & 0xFF );
    p = state + enc_len;

    /* Compute and write MAC( key_name + iv + enc_state_len + enc_state ) */
    sha256_hmac( ssl->ticket_keys->mac_key, 16, start, p - start, p, 0 );
    p += 32;

    *tlen = p - start;

    SSL_DEBUG_BUF( 3, "session ticket structure", start, *tlen );

    return( 0 );
}