static int ssl_parse_renegotiation_info( ssl_context *ssl,
                                         const unsigned char *buf,
                                         size_t len )
{
    int ret;

    if( ssl->renegotiation == SSL_INITIAL_HANDSHAKE )
    {
        if( len != 1 || buf[0] != 0x0 )
        {
            SSL_DEBUG_MSG( 1, ( "non-zero length renegotiated connection field" ) );

            if( ( ret = ssl_send_fatal_handshake_failure( ssl ) ) != 0 )
                return( ret );

            return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
        }

        ssl->secure_renegotiation = SSL_SECURE_RENEGOTIATION;
    }
    else
    {
        /* Check verify-data in constant-time. The length OTOH is no secret */
        if( len    != 1 + ssl->verify_data_len ||
            buf[0] !=     ssl->verify_data_len ||
            safer_memcmp( buf + 1, ssl->peer_verify_data,
                          ssl->verify_data_len ) != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "non-matching renegotiated connection field" ) );

            if( ( ret = ssl_send_fatal_handshake_failure( ssl ) ) != 0 )
                return( ret );

            return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
        }
    }

    return( 0 );
}