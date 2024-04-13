calc_enc_length_block (gnutls_session_t session, int data_size,
                 int hash_size, uint8_t * pad, 
                 unsigned auth_cipher, uint16_t blocksize)
{
  /* pad is the LH pad the user wants us to add. Besides
   * this LH pad, we only add minimal padding
   */
  unsigned int pre_length = data_size + hash_size + *pad;
  unsigned int length, new_pad;

  new_pad = (uint8_t) (blocksize - (pre_length % blocksize)) + *pad;
  
  if (new_pad > 255)
    new_pad -= blocksize;
  *pad = new_pad;

  length = data_size + hash_size + *pad;

  if (_gnutls_version_has_explicit_iv
      (session->security_parameters.version))
    length += blocksize;    /* for the IV */

  return length;
}