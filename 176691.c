void Curl_mbedtls_close(struct connectdata *conn, int sockindex)
{
  mbedtls_pk_free(&conn->ssl[sockindex].pk);
  mbedtls_x509_crt_free(&conn->ssl[sockindex].clicert);
  mbedtls_x509_crt_free(&conn->ssl[sockindex].cacert);
  mbedtls_x509_crl_free(&conn->ssl[sockindex].crl);
  mbedtls_ssl_config_free(&conn->ssl[sockindex].config);
  mbedtls_ssl_free(&conn->ssl[sockindex].ssl);
  mbedtls_ctr_drbg_free(&conn->ssl[sockindex].ctr_drbg);
#ifndef THREADING_SUPPORT
  mbedtls_entropy_free(&conn->ssl[sockindex].entropy);
#endif /* THREADING_SUPPORT */
}