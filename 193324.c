ciphertext_to_compressed (gnutls_session_t session,
                          gnutls_datum_t *ciphertext, 
                          opaque * compress_data,
                          int compress_size,
                          uint8_t type, record_parameters_st * params, 
                          uint64* sequence)
{
  uint8_t tag[MAX_HASH_SIZE];
  uint8_t pad;
  int length, length_to_decrypt;
  uint16_t blocksize;
  int ret, i, pad_failed = 0;
  opaque preamble[MAX_PREAMBLE_SIZE];
  int preamble_size;
  int ver = gnutls_protocol_get_version (session);
  int tag_size = _gnutls_auth_cipher_tag_len (&params->read.cipher_state);
  int explicit_iv = _gnutls_version_has_explicit_iv (session->security_parameters.version);

  blocksize = gnutls_cipher_get_block_size (params->cipher_algorithm);

  /* actual decryption (inplace)
   */
  switch (_gnutls_cipher_is_block (params->cipher_algorithm))
    {
    case CIPHER_STREAM:
      /* The way AEAD ciphers are defined in RFC5246, it allows
       * only stream ciphers.
       */
      if (explicit_iv && _gnutls_auth_cipher_is_aead(&params->read.cipher_state))
        {
          uint8_t nonce[blocksize];
          /* Values in AEAD are pretty fixed in TLS 1.2 for 128-bit block
           */
          if (params->read.IV.data == NULL || params->read.IV.size != 4)
            return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
          
          if (ciphertext->size < tag_size+AEAD_EXPLICIT_DATA_SIZE)
            return gnutls_assert_val(GNUTLS_E_UNEXPECTED_PACKET_LENGTH);

          memcpy(nonce, params->read.IV.data, AEAD_IMPLICIT_DATA_SIZE);
          memcpy(&nonce[AEAD_IMPLICIT_DATA_SIZE], ciphertext->data, AEAD_EXPLICIT_DATA_SIZE);
          
          _gnutls_auth_cipher_setiv(&params->read.cipher_state, nonce, AEAD_EXPLICIT_DATA_SIZE+AEAD_IMPLICIT_DATA_SIZE);

          ciphertext->data += AEAD_EXPLICIT_DATA_SIZE;
          ciphertext->size -= AEAD_EXPLICIT_DATA_SIZE;
          
          length_to_decrypt = ciphertext->size - tag_size;
        }
      else
        {
          if (ciphertext->size < tag_size)
            return gnutls_assert_val(GNUTLS_E_UNEXPECTED_PACKET_LENGTH);
  
          length_to_decrypt = ciphertext->size;
        }

      length = ciphertext->size - tag_size;

      /* Pass the type, version, length and compressed through
       * MAC.
       */
      preamble_size =
        make_preamble (UINT64DATA(*sequence), type,
                       length, ver, preamble);

      _gnutls_auth_cipher_add_auth (&params->read.cipher_state, preamble, preamble_size);

      if ((ret =
           _gnutls_auth_cipher_decrypt (&params->read.cipher_state,
             ciphertext->data, length_to_decrypt)) < 0)
        return gnutls_assert_val(ret);

      break;
    case CIPHER_BLOCK:
      if (ciphertext->size < MAX(blocksize, tag_size) || (ciphertext->size % blocksize != 0))
        return gnutls_assert_val(GNUTLS_E_UNEXPECTED_PACKET_LENGTH);

      /* ignore the IV in TLS 1.1+
       */
      if (explicit_iv)
        {
          _gnutls_auth_cipher_setiv(&params->read.cipher_state,
            ciphertext->data, blocksize);

          ciphertext->size -= blocksize;
          ciphertext->data += blocksize;

          if (ciphertext->size == 0)
            {
              gnutls_assert ();
              return GNUTLS_E_DECRYPTION_FAILED;
            }
        }

      /* we don't use the auth_cipher interface here, since
       * TLS with block ciphers is impossible to be used under such
       * an API. (the length of plaintext is required to calculate
       * auth_data, but it is not available before decryption).
       */
      if ((ret =
           _gnutls_cipher_decrypt (&params->read.cipher_state.cipher,
             ciphertext->data, ciphertext->size)) < 0)
        return gnutls_assert_val(ret);

      pad = ciphertext->data[ciphertext->size - 1] + 1;   /* pad */

      if ((int) pad > (int) ciphertext->size - tag_size)
        {
          gnutls_assert ();
          _gnutls_record_log
            ("REC[%p]: Short record length %d > %d - %d (under attack?)\n",
             session, pad, ciphertext->size, tag_size);
          /* We do not fail here. We check below for the
           * the pad_failed. If zero means success.
           */
          pad_failed = GNUTLS_E_DECRYPTION_FAILED;
          pad %= blocksize;
        }

      length = ciphertext->size - tag_size - pad;

      /* Check the pading bytes (TLS 1.x)
       */
      if (ver != GNUTLS_SSL3)
        for (i = 2; i < pad; i++)
          {
            if (ciphertext->data[ciphertext->size - i] !=
                ciphertext->data[ciphertext->size - 1])
              pad_failed = GNUTLS_E_DECRYPTION_FAILED;
          }

      if (length < 0)
        {
          /* Setting a proper length to prevent timing differences in
           * processing of records with invalid encryption.
           */
          length = ciphertext.size - tag_size;
        }

      /* Pass the type, version, length and compressed through
       * MAC.
       */
      preamble_size =
        make_preamble (UINT64DATA(*sequence), type,
                       length, ver, preamble);
      _gnutls_auth_cipher_add_auth (&params->read.cipher_state, preamble, preamble_size);
      _gnutls_auth_cipher_add_auth (&params->read.cipher_state, ciphertext->data, length);

      break;
    default:
      return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
    }

  ret = _gnutls_auth_cipher_tag(&params->read.cipher_state, tag, tag_size);
  if (ret < 0)
    return gnutls_assert_val(ret);

  /* This one was introduced to avoid a timing attack against the TLS
   * 1.0 protocol.
   */
  /* HMAC was not the same. 
   */
  if (memcmp (tag, &ciphertext->data[length], tag_size) != 0 || pad_failed != 0)
    return gnutls_assert_val(GNUTLS_E_DECRYPTION_FAILED);

  /* copy the decrypted stuff to compress_data.
   */
  if (compress_size < length)
    return gnutls_assert_val(GNUTLS_E_DECOMPRESSION_FAILED);

  if (compress_data != ciphertext->data)
    memcpy (compress_data, ciphertext->data, length);

  return length;
}