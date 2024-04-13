static int ssl_ciphersuite_match( ssl_context *ssl, int suite_id,
                                  const ssl_ciphersuite_t **ciphersuite_info )
{
    const ssl_ciphersuite_t *suite_info;

    suite_info = ssl_ciphersuite_from_id( suite_id );
    if( suite_info == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "ciphersuite info for %04x not found", suite_id ) );
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );
    }

    if( suite_info->min_minor_ver > ssl->minor_ver ||
        suite_info->max_minor_ver < ssl->minor_ver )
        return( 0 );

#if defined(POLARSSL_ECDH_C) || defined(POLARSSL_ECDSA_C)
    if( ssl_ciphersuite_uses_ec( suite_info ) &&
        ( ssl->handshake->curves == NULL ||
          ssl->handshake->curves[0] == NULL ) )
        return( 0 );
#endif

#if defined(POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED)
    /* If the ciphersuite requires a pre-shared key and we don't
     * have one, skip it now rather than failing later */
    if( ssl_ciphersuite_uses_psk( suite_info ) &&
        ssl->f_psk == NULL &&
        ( ssl->psk == NULL || ssl->psk_identity == NULL ||
          ssl->psk_identity_len == 0 || ssl->psk_len == 0 ) )
        return( 0 );
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
    /*
     * Final check: if ciphersuite requires us to have a
     * certificate/key of a particular type:
     * - select the appropriate certificate if we have one, or
     * - try the next ciphersuite if we don't
     * This must be done last since we modify the key_cert list.
     */
    if( ssl_pick_cert( ssl, suite_info ) != 0 )
        return( 0 );
#endif

    *ciphersuite_info = suite_info;
    return( 0 );
}