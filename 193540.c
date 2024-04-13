static int ssl_parse_truncated_hmac_ext( ssl_context *ssl,
                                         const unsigned char *buf,
                                         size_t len )
{
    if( len != 0 )
    {
        SSL_DEBUG_MSG( 1, ( "bad client hello message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
    }

    ((void) buf);

    ssl->session_negotiate->trunc_hmac = SSL_TRUNC_HMAC_ENABLED;

    return( 0 );
}