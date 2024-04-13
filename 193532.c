static int ssl_parse_max_fragment_length_ext( ssl_context *ssl,
                                              const unsigned char *buf,
                                              size_t len )
{
    if( len != 1 || buf[0] >= SSL_MAX_FRAG_LEN_INVALID )
    {
        SSL_DEBUG_MSG( 1, ( "bad client hello message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
    }

    ssl->session_negotiate->mfl_code = buf[0];

    return( 0 );
}