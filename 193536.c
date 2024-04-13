static int ssl_sni_wrapper( ssl_context *ssl,
                            const unsigned char* name, size_t len )
{
    int ret;
    ssl_key_cert *key_cert_ori = ssl->key_cert;

    ssl->key_cert = NULL;
    ret = ssl->f_sni( ssl->p_sni, ssl, name, len );
    ssl->handshake->sni_key_cert = ssl->key_cert;

    ssl->key_cert = key_cert_ori;

    return( ret );
}