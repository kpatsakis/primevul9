mbed_connect_step2(struct connectdata *conn,
                   int sockindex)
{
  int ret;
  struct SessionHandle *data = conn->data;
  struct ssl_connect_data* connssl = &conn->ssl[sockindex];
  const mbedtls_x509_crt *peercert;

#ifdef HAS_ALPN
  const char* next_protocol;
#endif

  char errorbuf[128];
  errorbuf[0] = 0;

  conn->recv[sockindex] = mbed_recv;
  conn->send[sockindex] = mbed_send;

  ret = mbedtls_ssl_handshake(&connssl->ssl);

  if(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->connecting_state = ssl_connect_2_reading;
    return CURLE_OK;
  }
  else if(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->connecting_state = ssl_connect_2_writing;
    return CURLE_OK;
  }
  else if(ret) {
#ifdef MBEDTLS_ERROR_C
    mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
#endif /* MBEDTLS_ERROR_C */
    failf(data, "ssl_handshake returned - mbedTLS: (-0x%04X) %s",
          -ret, errorbuf);
    return CURLE_SSL_CONNECT_ERROR;
  }

  infof(data, "mbedTLS: Handshake complete, cipher is %s\n",
        mbedtls_ssl_get_ciphersuite(&conn->ssl[sockindex].ssl)
    );

  ret = mbedtls_ssl_get_verify_result(&conn->ssl[sockindex].ssl);

  if(ret && data->set.ssl.verifypeer) {
    if(ret & MBEDTLS_X509_BADCERT_EXPIRED)
      failf(data, "Cert verify failed: BADCERT_EXPIRED");

    if(ret & MBEDTLS_X509_BADCERT_REVOKED) {
      failf(data, "Cert verify failed: BADCERT_REVOKED");
      return CURLE_SSL_CACERT;
    }

    if(ret & MBEDTLS_X509_BADCERT_CN_MISMATCH)
      failf(data, "Cert verify failed: BADCERT_CN_MISMATCH");

    if(ret & MBEDTLS_X509_BADCERT_NOT_TRUSTED)
      failf(data, "Cert verify failed: BADCERT_NOT_TRUSTED");

    return CURLE_PEER_FAILED_VERIFICATION;
  }

  peercert = mbedtls_ssl_get_peer_cert(&connssl->ssl);

  if(peercert && data->set.verbose) {
    const size_t bufsize = 16384;
    char *buffer = malloc(bufsize);

    if(!buffer)
      return CURLE_OUT_OF_MEMORY;

    if(mbedtls_x509_crt_info(buffer, bufsize, "* ", peercert) > 0)
      infof(data, "Dumping cert info:\n%s\n", buffer);
    else
      infof(data, "Unable to dump certificate information.\n");

    free(buffer);
  }

  if(data->set.str[STRING_SSL_PINNEDPUBLICKEY]) {
    int size;
    CURLcode result;
    mbedtls_x509_crt *p;
    unsigned char pubkey[PUB_DER_MAX_BYTES];

    if(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      return CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = calloc(1, sizeof(*p));

    if(!p)
      return CURLE_OUT_OF_MEMORY;

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/ARMmbed/mbedtls/issues/396 */
    if(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      mbedtls_x509_crt_free(p);
      free(p);
      return CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    if(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      mbedtls_x509_crt_free(p);
      free(p);
      return CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  data->set.str[STRING_SSL_PINNEDPUBLICKEY],
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
    if(result) {
      mbedtls_x509_crt_free(p);
      free(p);
      return result;
    }

    mbedtls_x509_crt_free(p);
    free(p);
  }

#ifdef HAS_ALPN
  if(conn->bits.tls_enable_alpn) {
    next_protocol = mbedtls_ssl_get_alpn_protocol(&connssl->ssl);

    if(next_protocol) {
      infof(data, "ALPN, server accepted to use %s\n", next_protocol);
#ifdef USE_NGHTTP2
      if(!strncmp(next_protocol, NGHTTP2_PROTO_VERSION_ID,
                  NGHTTP2_PROTO_VERSION_ID_LEN) &&
         !next_protocol[NGHTTP2_PROTO_VERSION_ID_LEN]) {
        conn->negnpn = CURL_HTTP_VERSION_2;
      }
      else
#endif
        if(!strncmp(next_protocol, ALPN_HTTP_1_1, ALPN_HTTP_1_1_LENGTH) &&
           !next_protocol[ALPN_HTTP_1_1_LENGTH]) {
          conn->negnpn = CURL_HTTP_VERSION_1_1;
        }
    }
    else {
      infof(data, "ALPN, server did not agree to a protocol\n");
    }
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected\n");

  return CURLE_OK;
}