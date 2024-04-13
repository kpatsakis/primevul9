static int ssl_parse_servername_ext( ssl_context *ssl,
                                     const unsigned char *buf,
                                     size_t len )
{
    int ret;
    size_t servername_list_size, hostname_len;
    const unsigned char *p;

    SSL_DEBUG_MSG( 3, ( "parse ServerName extension" ) );

    servername_list_size = ( ( buf[0] << 8 ) | ( buf[1] ) );
    if( servername_list_size + 2 != len )
    {
        SSL_DEBUG_MSG( 1, ( "bad client hello message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
    }

    p = buf + 2;
    while( servername_list_size > 0 )
    {
        hostname_len = ( ( p[1] << 8 ) | p[2] );
        if( hostname_len + 3 > servername_list_size )
        {
            SSL_DEBUG_MSG( 1, ( "bad client hello message" ) );
            return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
        }

        if( p[0] == TLS_EXT_SERVERNAME_HOSTNAME )
        {
            ret = ssl_sni_wrapper( ssl, p + 3, hostname_len );
            if( ret != 0 )
            {
                SSL_DEBUG_RET( 1, "ssl_sni_wrapper", ret );
                ssl_send_alert_message( ssl, SSL_ALERT_LEVEL_FATAL,
                        SSL_ALERT_MSG_UNRECOGNIZED_NAME );
                return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
            }
            return( 0 );
        }

        servername_list_size -= hostname_len + 3;
        p += hostname_len + 3;
    }

    if( servername_list_size != 0 )
    {
        SSL_DEBUG_MSG( 1, ( "bad client hello message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
    }

    return( 0 );
}