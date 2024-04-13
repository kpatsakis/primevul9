calc_enc_length_stream (gnutls_session_t session, int data_size,
                 int hash_size, unsigned auth_cipher)
{
  unsigned int length;

  length = data_size + hash_size;
  if (auth_cipher)
    length += AEAD_EXPLICIT_DATA_SIZE;

  return length;
}