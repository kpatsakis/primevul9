void Curl_polarssl_close(struct connectdata *conn, int sockindex)
{
  rsa_free(&conn->ssl[sockindex].rsa);
  x509_crt_free(&conn->ssl[sockindex].clicert);
  x509_crt_free(&conn->ssl[sockindex].cacert);
  x509_crl_free(&conn->ssl[sockindex].crl);
  ssl_free(&conn->ssl[sockindex].ssl);
}