static int ssl_parse_ticket( ssl_context *ssl,
                             unsigned char *buf,
                             size_t len )
{
    int ret;
    ssl_session session;
    unsigned char *key_name = buf;
    unsigned char *iv = buf + 16;
    unsigned char *enc_len_p = iv + 16;
    unsigned char *ticket = enc_len_p + 2;
    unsigned char *mac;
    unsigned char computed_mac[32];
    size_t enc_len, clear_len, i;
    unsigned char pad_len, diff;

    SSL_DEBUG_BUF( 3, "session ticket structure", buf, len );

    if( len < 34 || ssl->ticket_keys == NULL )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    enc_len = ( enc_len_p[0] << 8 ) | enc_len_p[1];
    mac = ticket + enc_len;

    if( len != enc_len + 66 )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    /* Check name, in constant time though it's not a big secret */
    diff = 0;
    for( i = 0; i < 16; i++ )
        diff |= key_name[i] ^ ssl->ticket_keys->key_name[i];
    /* don't return yet, check the MAC anyway */

    /* Check mac, with constant-time buffer comparison */
    sha256_hmac( ssl->ticket_keys->mac_key, 16, buf, len - 32,
                 computed_mac, 0 );

    for( i = 0; i < 32; i++ )
        diff |= mac[i] ^ computed_mac[i];

    /* Now return if ticket is not authentic, since we want to avoid
     * decrypting arbitrary attacker-chosen data */
    if( diff != 0 )
        return( POLARSSL_ERR_SSL_INVALID_MAC );

    /* Decrypt */
    if( ( ret = aes_crypt_cbc( &ssl->ticket_keys->dec, AES_DECRYPT,
                               enc_len, iv, ticket, ticket ) ) != 0 )
    {
        return( ret );
    }

    /* Check PKCS padding */
    pad_len = ticket[enc_len - 1];

    ret = 0;
    for( i = 2; i < pad_len; i++ )
        if( ticket[enc_len - i] != pad_len )
            ret = POLARSSL_ERR_SSL_BAD_INPUT_DATA;
    if( ret != 0 )
        return( ret );

    clear_len = enc_len - pad_len;

    SSL_DEBUG_BUF( 3, "session ticket cleartext", ticket, clear_len );

    /* Actually load session */
    if( ( ret = ssl_load_session( &session, ticket, clear_len ) ) != 0 )
    {
        SSL_DEBUG_MSG( 1, ( "failed to parse ticket content" ) );
        ssl_session_free( &session );
        return( ret );
    }

#if defined(POLARSSL_HAVE_TIME)
    /* Check if still valid */
    if( (int) ( time( NULL) - session.start ) > ssl->ticket_lifetime )
    {
        SSL_DEBUG_MSG( 1, ( "session ticket expired" ) );
        ssl_session_free( &session );
        return( POLARSSL_ERR_SSL_SESSION_TICKET_EXPIRED );
    }
#endif

    /*
     * Keep the session ID sent by the client, since we MUST send it back to
     * inform him we're accepting the ticket  (RFC 5077 section 3.4)
     */
    session.length = ssl->session_negotiate->length;
    memcpy( &session.id, ssl->session_negotiate->id, session.length );

    ssl_session_free( ssl->session_negotiate );
    memcpy( ssl->session_negotiate, &session, sizeof( ssl_session ) );

    /* Zeroize instead of free as we copied the content */
    polarssl_zeroize( &session, sizeof( ssl_session ) );

    return( 0 );
}