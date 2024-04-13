static int nb_add_signature (lcc_network_buffer_t *nb) /* {{{ */
{
  char *buffer;
  size_t buffer_size;

  gcry_md_hd_t hd;
  gcry_error_t err;
  unsigned char *hash;
  const size_t hash_length = 32;

  /* The type, length and username have already been filled in by
   * "lcc_network_buffer_initialize". All we do here is calculate the hash over
   * the username and the data and add the hash value to the buffer. */

  buffer = nb->buffer + PART_SIGNATURE_SHA256_SIZE;
  assert (nb->size >= (nb->free + PART_SIGNATURE_SHA256_SIZE));
  buffer_size = nb->size - (nb->free + PART_SIGNATURE_SHA256_SIZE);

  hd = NULL;
  err = gcry_md_open (&hd, GCRY_MD_SHA256, GCRY_MD_FLAG_HMAC);
  if (err != 0)
    return (-1);

  assert (nb->password != NULL);
  err = gcry_md_setkey (hd, nb->password, strlen (nb->password));
  if (err != 0)
  {
    gcry_md_close (hd);
    return (-1);
  }

  gcry_md_write (hd, buffer, buffer_size);
  hash = gcry_md_read (hd, GCRY_MD_SHA256);
  if (hash == NULL)
  {
    gcry_md_close (hd);
    return (-1);
  }

  assert (((2 * sizeof (uint16_t)) + hash_length) == PART_SIGNATURE_SHA256_SIZE);
  memcpy (nb->buffer + (2 * sizeof (uint16_t)), hash, hash_length);

  gcry_md_close (hd);
  return (0);
} /* }}} int nb_add_signature */