static int ssl_load_session( ssl_session *session,
                             const unsigned char *buf, size_t len )
{
    const unsigned char *p = buf;
    const unsigned char * const end = buf + len;
#if defined(POLARSSL_X509_CRT_PARSE_C)
    size_t cert_len;
#endif /* POLARSSL_X509_CRT_PARSE_C */

    if( p + sizeof( ssl_session ) > end )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    memcpy( session, p, sizeof( ssl_session ) );
    p += sizeof( ssl_session );

#if defined(POLARSSL_X509_CRT_PARSE_C)
    if( p + 3 > end )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    cert_len = ( p[0] << 16 ) | ( p[1] << 8 ) | p[2];
    p += 3;

    if( cert_len == 0 )
    {
        session->peer_cert = NULL;
    }
    else
    {
        int ret;

        if( p + cert_len > end )
            return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

        session->peer_cert = polarssl_malloc( sizeof( x509_crt ) );

        if( session->peer_cert == NULL )
            return( POLARSSL_ERR_SSL_MALLOC_FAILED );

        x509_crt_init( session->peer_cert );

        if( ( ret = x509_crt_parse_der( session->peer_cert,
                                        p, cert_len ) ) != 0 )
        {
            x509_crt_free( session->peer_cert );
            polarssl_free( session->peer_cert );
            session->peer_cert = NULL;
            return( ret );
        }

        p += cert_len;
    }
#endif /* POLARSSL_X509_CRT_PARSE_C */

    if( p != end )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    return( 0 );
}