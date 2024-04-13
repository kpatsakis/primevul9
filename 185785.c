ciphertext_to_compressed_new (gnutls_session_t session,
                              gnutls_datum_t *ciphertext, 
                              gnutls_datum_t * compressed,
                              uint8_t type, record_parameters_st * params, 
                              uint64* sequence)
{
  uint8_t tag[MAX_HASH_SIZE];
  uint8_t *tag_ptr;
  unsigned int pad;
  int length, length_to_decrypt;
  uint16_t blocksize;
  int ret;
  uint8_t preamble[MAX_PREAMBLE_SIZE];
  unsigned int preamble_size;
  unsigned int ver = gnutls_protocol_get_version (session);
  unsigned int tag_size = _gnutls_auth_cipher_tag_len (&params->read.cipher_state);
  unsigned int explicit_iv = _gnutls_version_has_explicit_iv (session->security_parameters.version);

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
          
          if (ciphertext->size < tag_size+AEAD_EXPLICIT_DATA_SIZE + 2)
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
      break;
    case CIPHER_BLOCK:
      if (ciphertext->size < blocksize || (ciphertext->size % blocksize != 0))
        return gnutls_assert_val(GNUTLS_E_UNEXPECTED_PACKET_LENGTH);

      if (explicit_iv)
        {
          _gnutls_auth_cipher_setiv(&params->read.cipher_state,
            ciphertext->data, blocksize);

          ciphertext->size -= blocksize;
          ciphertext->data += blocksize;
        }

      if (ciphertext->size < tag_size + 2)
        return gnutls_assert_val(GNUTLS_E_DECRYPTION_FAILED);
      
      length_to_decrypt = ciphertext->size;
      break;

    default:
      return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
    }

  length = ciphertext->size - tag_size;

  preamble_size =
    make_preamble (UINT64DATA(*sequence), type,
                   length, ver, preamble);

  ret = _gnutls_auth_cipher_add_auth (&params->read.cipher_state, preamble, preamble_size);
  if (ret < 0)
    return gnutls_assert_val(ret);

  ret =
       _gnutls_auth_cipher_decrypt2 (&params->read.cipher_state,
                                     ciphertext->data, length_to_decrypt,
                                     ciphertext->data, ciphertext->size);
  if (ret < 0)
    return gnutls_assert_val(ret);

  pad = _gnutls_read_uint16(ciphertext->data);

  tag_ptr = &ciphertext->data[length];
  ret = _gnutls_auth_cipher_tag(&params->read.cipher_state, tag, tag_size);
  if (ret < 0)
    return gnutls_assert_val(ret);

  /* Check MAC.
   */
  if (memcmp (tag, tag_ptr, tag_size) != 0)
    return gnutls_assert_val(GNUTLS_E_DECRYPTION_FAILED);

  DECR_LEN(length, 2+pad);

  /* copy the decrypted stuff to compress_data.
   */
  if (compressed->size < (unsigned)length)
    return gnutls_assert_val(GNUTLS_E_DECOMPRESSION_FAILED);

  memcpy (compressed->data, &ciphertext->data[2+pad], length);

  return length;
}