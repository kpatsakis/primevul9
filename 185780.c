compressed_to_ciphertext_new (gnutls_session_t session,
                              uint8_t * cipher_data, int cipher_size,
                              gnutls_datum_t *compressed,
                              size_t target_size,
                              content_type_t type, 
                              record_parameters_st * params)
{
  uint8_t * tag_ptr = NULL;
  uint16_t pad = target_size - compressed->size;
  int length, length_to_encrypt, ret;
  uint8_t preamble[MAX_PREAMBLE_SIZE];
  int preamble_size;
  int tag_size = _gnutls_auth_cipher_tag_len (&params->write.cipher_state);
  int blocksize = gnutls_cipher_get_block_size (params->cipher_algorithm);
  unsigned block_algo =
    _gnutls_cipher_is_block (params->cipher_algorithm);
  uint8_t *data_ptr;
  int ver = gnutls_protocol_get_version (session);
  int explicit_iv = _gnutls_version_has_explicit_iv (session->security_parameters.version);
  int auth_cipher = _gnutls_auth_cipher_is_aead(&params->write.cipher_state);
  uint8_t nonce[MAX_CIPHER_BLOCK_SIZE];

  _gnutls_hard_log("ENC[%p]: cipher: %s, MAC: %s, Epoch: %u\n",
    session, gnutls_cipher_get_name(params->cipher_algorithm), gnutls_mac_get_name(params->mac_algorithm),
    (unsigned int)params->epoch);

  /* Call _gnutls_rnd() once. Get data used for the IV
   */
  ret = _gnutls_rnd (GNUTLS_RND_NONCE, nonce, blocksize);
  if (ret < 0)
    return gnutls_assert_val(ret);

  /* cipher_data points to the start of data to be encrypted */
  data_ptr = cipher_data;

  length_to_encrypt = length = 0;

  if (explicit_iv)
    {
      if (block_algo == CIPHER_BLOCK)
        {
          /* copy the random IV.
           */
          DECR_LEN(cipher_size, blocksize);

          memcpy(data_ptr, nonce, blocksize);
          _gnutls_auth_cipher_setiv(&params->write.cipher_state, data_ptr, blocksize);

          data_ptr += blocksize;
          cipher_data += blocksize;
          length += blocksize;
        }
      else if (auth_cipher)
        {
          /* Values in AEAD are pretty fixed in TLS 1.2 for 128-bit block
           */
          if (params->write.IV.data == NULL || params->write.IV.size != AEAD_IMPLICIT_DATA_SIZE)
            return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

          /* Instead of generating a new nonce on every packet, we use the
           * write.sequence_number (It is a MAY on RFC 5288).
           */
          memcpy(nonce, params->write.IV.data, params->write.IV.size);
          memcpy(&nonce[AEAD_IMPLICIT_DATA_SIZE], UINT64DATA(params->write.sequence_number), 8);

          _gnutls_auth_cipher_setiv(&params->write.cipher_state, nonce, AEAD_IMPLICIT_DATA_SIZE+AEAD_EXPLICIT_DATA_SIZE);

          /* copy the explicit part */
          DECR_LEN(cipher_size, AEAD_EXPLICIT_DATA_SIZE);
          memcpy(data_ptr, &nonce[AEAD_IMPLICIT_DATA_SIZE], AEAD_EXPLICIT_DATA_SIZE);

          data_ptr += AEAD_EXPLICIT_DATA_SIZE;
          cipher_data += AEAD_EXPLICIT_DATA_SIZE;
          length += AEAD_EXPLICIT_DATA_SIZE;
        }
    }
  else
    {
      /* AEAD ciphers have an explicit IV. Shouldn't be used otherwise.
       */
      if (auth_cipher) return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
    }

  DECR_LEN(cipher_size, 2);

  if (block_algo == CIPHER_BLOCK) /* make pad a multiple of blocksize */
    {
      unsigned t = (2 + pad + compressed->size + tag_size) % blocksize;
      if (t > 0)
        {
    	  pad += blocksize - t;
        }
    }
  
  _gnutls_write_uint16 (pad, data_ptr);
  data_ptr += 2;
  length_to_encrypt += 2;
  length += 2;
 
  if (pad > 0)
    { 
      DECR_LEN(cipher_size, pad);
      memset(data_ptr, 0, pad);
      data_ptr += pad;
      length_to_encrypt += pad;
      length += pad;
    }

  DECR_LEN(cipher_size, compressed->size);
  memcpy (data_ptr, compressed->data, compressed->size);
  data_ptr += compressed->size;
  length_to_encrypt += compressed->size;
  length += compressed->size;

  if (tag_size > 0)
    {
      DECR_LEN(cipher_size, tag_size);
      tag_ptr = data_ptr;
      data_ptr += tag_size;
      
      /* In AEAD ciphers we don't encrypt the tag 
       */
      if (!auth_cipher)
        length_to_encrypt += tag_size;
      length += tag_size;
    }

  preamble_size =
    make_preamble (UINT64DATA
                   (params->write.sequence_number),
                   type, compressed->size+2+pad, ver, preamble);

  /* add the authenticated data */
  ret = _gnutls_auth_cipher_add_auth(&params->write.cipher_state, preamble, preamble_size);
  if (ret < 0)
    return gnutls_assert_val(ret);

  /* Actual encryption (inplace).
   */
  ret =
    _gnutls_auth_cipher_encrypt2_tag (&params->write.cipher_state,
        cipher_data, length_to_encrypt, 
        cipher_data, cipher_size,
        tag_ptr, tag_size, compressed->size+2+pad);
  if (ret < 0)
    return gnutls_assert_val(ret);

  return length;
}