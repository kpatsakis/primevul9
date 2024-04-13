static int ssl_parse_signature_algorithms_ext( ssl_context *ssl,
                                               const unsigned char *buf,
                                               size_t len )
{
    size_t sig_alg_list_size;
    const unsigned char *p;
    const unsigned char *end = buf + len;
    const int *md_cur;


    sig_alg_list_size = ( ( buf[0] << 8 ) | ( buf[1] ) );
    if( sig_alg_list_size + 2 != len ||
        sig_alg_list_size % 2 != 0 )
    {
        SSL_DEBUG_MSG( 1, ( "bad client hello message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO );
    }

    /*
     * For now, ignore the SignatureAlgorithm part and rely on offered
     * ciphersuites only for that part. To be fixed later.
     *
     * So, just look at the HashAlgorithm part.
     */
    for( md_cur = md_list(); *md_cur != POLARSSL_MD_NONE; md_cur++ ) {
        for( p = buf + 2; p < end; p += 2 ) {
            if( *md_cur == (int) ssl_md_alg_from_hash( p[0] ) ) {
                ssl->handshake->sig_alg = p[0];
                goto have_sig_alg;
            }
        }
    }

    /* Some key echanges do not need signatures at all */
    SSL_DEBUG_MSG( 3, ( "no signature_algorithm in common" ) );
    return( 0 );

have_sig_alg:
    SSL_DEBUG_MSG( 3, ( "client hello v3, signature_algorithm ext: %d",
                   ssl->handshake->sig_alg ) );

    return( 0 );
}