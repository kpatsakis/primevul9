calc_enc_length (gnutls_session_t session, int data_size,
                 int hash_size, uint8_t * pad, int random_pad,
                 unsigned block_algo, unsigned auth_cipher, uint16_t blocksize)
{
  uint8_t rnd;
  unsigned int length;
  int ret;

  *pad = 0;

  switch (block_algo)
    {
    case CIPHER_STREAM:
      length = data_size + hash_size;
      if (auth_cipher)
        length += AEAD_EXPLICIT_DATA_SIZE;

      break;
    case CIPHER_BLOCK:
      ret = _gnutls_rnd (GNUTLS_RND_NONCE, &rnd, 1);
      if (ret < 0)
        return gnutls_assert_val(ret);

      /* make rnd a multiple of blocksize */
      if (session->security_parameters.version == GNUTLS_SSL3 ||
          random_pad == 0)
        {
          rnd = 0;
        }
      else
        {
          rnd = (rnd / blocksize) * blocksize;
          /* added to avoid the case of pad calculated 0
           * seen below for pad calculation.
           */
          if (rnd > blocksize)
            rnd -= blocksize;
        }

      length = data_size + hash_size;

      *pad = (uint8_t) (blocksize - (length % blocksize)) + rnd;

      length += *pad;
      if (_gnutls_version_has_explicit_iv
          (session->security_parameters.version))
        length += blocksize;    /* for the IV */

      break;
    default:
      return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
    }

  return length;
}