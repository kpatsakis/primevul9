polarssl_connect_step2(struct connectdata *conn,
                     int sockindex)
{
  int ret;
  struct SessionHandle *data = conn->data;
  struct ssl_connect_data* connssl = &conn->ssl[sockindex];
  char buffer[1024];

  char errorbuf[128];
  errorbuf[0] = 0;

  conn->recv[sockindex] = polarssl_recv;
  conn->send[sockindex] = polarssl_send;

  ret = ssl_handshake(&connssl->ssl);

  switch(ret) {
  case 0:
    break;

  case POLARSSL_ERR_NET_WANT_READ:
    connssl->connecting_state = ssl_connect_2_reading;
    return CURLE_OK;

  case POLARSSL_ERR_NET_WANT_WRITE:
    connssl->connecting_state = ssl_connect_2_writing;
    return CURLE_OK;

  default:
#ifdef POLARSSL_ERROR_C
    error_strerror(ret, errorbuf, sizeof(errorbuf));
#endif /* POLARSSL_ERROR_C */
    failf(data, "ssl_handshake returned - PolarSSL: (-0x%04X) %s",
          -ret, errorbuf);
    return CURLE_SSL_CONNECT_ERROR;
  }

  infof(data, "PolarSSL: Handshake complete, cipher is %s\n",
        ssl_get_ciphersuite(&conn->ssl[sockindex].ssl) );

  ret = ssl_get_verify_result(&conn->ssl[sockindex].ssl);

  if(ret && data->set.ssl.verifypeer) {
    if(ret & BADCERT_EXPIRED)
      failf(data, "Cert verify failed: BADCERT_EXPIRED");

    if(ret & BADCERT_REVOKED) {
      failf(data, "Cert verify failed: BADCERT_REVOKED");
      return CURLE_SSL_CACERT;
    }

    if(ret & BADCERT_CN_MISMATCH)
      failf(data, "Cert verify failed: BADCERT_CN_MISMATCH");

    if(ret & BADCERT_NOT_TRUSTED)
      failf(data, "Cert verify failed: BADCERT_NOT_TRUSTED");

    return CURLE_PEER_FAILED_VERIFICATION;
  }

  if(ssl_get_peer_cert(&(connssl->ssl))) {
    /* If the session was resumed, there will be no peer certs */
    memset(buffer, 0, sizeof(buffer));

    if(x509_crt_info(buffer, sizeof(buffer), (char *)"* ",
                     ssl_get_peer_cert(&(connssl->ssl))) != -1)
      infof(data, "Dumping cert info:\n%s\n", buffer);
  }

  /* adapted from mbedtls.c */
  if(data->set.str[STRING_SSL_PINNEDPUBLICKEY]) {
    int size;
    CURLcode result;
    x509_crt *p;
    unsigned char pubkey[PUB_DER_MAX_BYTES];
    const x509_crt *peercert;

    peercert = ssl_get_peer_cert(&connssl->ssl);

    if(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      return CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = calloc(1, sizeof(*p));

    if(!p)
      return CURLE_OUT_OF_MEMORY;

    x509_crt_init(p);

    /* Make a copy of our const peercert because pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/ARMmbed/mbedtls/issues/396 */
    if(x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      x509_crt_free(p);
      free(p);
      return CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    size = pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    if(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      x509_crt_free(p);
      free(p);
      return CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    /* pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  data->set.str[STRING_SSL_PINNEDPUBLICKEY],
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
    if(result) {
      x509_crt_free(p);
      free(p);
      return result;
    }

    x509_crt_free(p);
    free(p);
  }

#ifdef HAS_ALPN
  if(conn->bits.tls_enable_alpn) {
    const char *next_protocol = ssl_get_alpn_protocol(&connssl->ssl);

    if(next_protocol != NULL) {
      infof(data, "ALPN, server accepted to use %s\n", next_protocol);

#ifdef USE_NGHTTP2
      if(!strncmp(next_protocol, NGHTTP2_PROTO_VERSION_ID,
                  NGHTTP2_PROTO_VERSION_ID_LEN)) {
        conn->negnpn = CURL_HTTP_VERSION_2;
      }
      else
#endif
      if(!strncmp(next_protocol, ALPN_HTTP_1_1, ALPN_HTTP_1_1_LENGTH)) {
        conn->negnpn = CURL_HTTP_VERSION_1_1;
      }
    }
    else
      infof(data, "ALPN, server did not agree to a protocol\n");
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected\n");

  return CURLE_OK;
}