static int ssl_save_session( const ssl_session *session,
                             unsigned char *buf, size_t buf_len,
                             size_t *olen )
{
    unsigned char *p = buf;
    size_t left = buf_len;
#if defined(POLARSSL_X509_CRT_PARSE_C)
    size_t cert_len;
#endif /* POLARSSL_X509_CRT_PARSE_C */

    if( left < sizeof( ssl_session ) )
        return( -1 );

    memcpy( p, session, sizeof( ssl_session ) );
    p += sizeof( ssl_session );
    left -= sizeof( ssl_session );

#if defined(POLARSSL_X509_CRT_PARSE_C)
    if( session->peer_cert == NULL )
        cert_len = 0;
    else
        cert_len = session->peer_cert->raw.len;

    if( left < 3 + cert_len )
        return( -1 );

    *p++ = (unsigned char)( cert_len >> 16 & 0xFF );
    *p++ = (unsigned char)( cert_len >>  8 & 0xFF );
    *p++ = (unsigned char)( cert_len       & 0xFF );

    if( session->peer_cert != NULL )
        memcpy( p, session->peer_cert->raw.p, cert_len );

    p += cert_len;
#endif /* POLARSSL_X509_CRT_PARSE_C */

    *olen = p - buf;

    return( 0 );
}