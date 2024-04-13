static int ssl_pick_cert( ssl_context *ssl,
                          const ssl_ciphersuite_t * ciphersuite_info )
{
    ssl_key_cert *cur, *list;
    pk_type_t pk_alg = ssl_get_ciphersuite_sig_pk_alg( ciphersuite_info );

#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    if( ssl->handshake->sni_key_cert != NULL )
        list = ssl->handshake->sni_key_cert;
    else
#endif
        list = ssl->handshake->key_cert;

    if( pk_alg == POLARSSL_PK_NONE )
        return( 0 );

    for( cur = list; cur != NULL; cur = cur->next )
    {
        if( ! pk_can_do( cur->key, pk_alg ) )
            continue;

        /*
         * This avoids sending the client a cert it'll reject based on
         * keyUsage or other extensions.
         *
         * It also allows the user to provision different certificates for
         * different uses based on keyUsage, eg if they want to avoid signing
         * and decrypting with the same RSA key.
         */
        if( ssl_check_cert_usage( cur->cert, ciphersuite_info,
                                  SSL_IS_SERVER ) != 0 )
        {
            continue;
        }

#if defined(POLARSSL_ECDSA_C)
        if( pk_alg == POLARSSL_PK_ECDSA )
        {
            if( ssl_key_matches_curves( cur->key, ssl->handshake->curves ) )
                break;
        }
        else
#endif
            break;
    }

    if( cur == NULL )
        return( -1 );

    ssl->handshake->key_cert = cur;
    return( 0 );
}