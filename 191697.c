static int nb_add_encryption (lcc_network_buffer_t *nb) /* {{{ */
{
  size_t package_length;
  char *encr_ptr; /* pointer to data being encrypted */
  size_t encr_size;

  char *hash_ptr; /* pointer to data being hashed */
  size_t hash_size;
  char hash[20];

  uint16_t pkg_length;
  gcry_error_t err;

  /* Fill in the package length */
  package_length = nb->size - nb->free;
  pkg_length = htons ((uint16_t) package_length);
  memcpy (nb->buffer + 2, &pkg_length, sizeof (pkg_length));

  /* Calculate what to hash */
  hash_ptr = nb->buffer + PART_ENCRYPTION_AES256_SIZE;
  hash_size = package_length - nb->encr_header_len;

  /* Calculate what to encrypt */
  encr_ptr = hash_ptr - sizeof (hash);
  encr_size = hash_size + sizeof (hash);

  /* Calculate the SHA-1 hash */
  gcry_md_hash_buffer (GCRY_MD_SHA1, hash, hash_ptr, hash_size);
  memcpy (encr_ptr, hash, sizeof (hash));

  if (nb->encr_cypher == NULL)
  {
    unsigned char password_hash[32];

    err = gcry_cipher_open (&nb->encr_cypher,
        GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_OFB, /* flags = */ 0);
    if (err != 0)
      return (-1);

    /* Calculate our 256bit key used for AES */
    gcry_md_hash_buffer (GCRY_MD_SHA256, password_hash,
        nb->password, strlen (nb->password));

    err = gcry_cipher_setkey (nb->encr_cypher,
        password_hash, sizeof (password_hash));
    if (err != 0)
    {
      gcry_cipher_close (nb->encr_cypher);
      nb->encr_cypher = NULL;
      return (-1);
    }
  }
  else /* if (nb->encr_cypher != NULL) */
  {
    gcry_cipher_reset (nb->encr_cypher);
  }

  /* Set the initialization vector */
  err = gcry_cipher_setiv (nb->encr_cypher,
      nb->encr_iv, sizeof (nb->encr_iv));
  if (err != 0)
  {
    gcry_cipher_close (nb->encr_cypher);
    nb->encr_cypher = NULL;
    return (-1);
  }

  /* Encrypt the buffer in-place */
  err = gcry_cipher_encrypt (nb->encr_cypher,
      encr_ptr, encr_size,
      /* in = */ NULL, /* in len = */ 0);
  if (err != 0)
  {
    gcry_cipher_close (nb->encr_cypher);
    nb->encr_cypher = NULL;
    return (-1);
  }

  return (0);
} /* }}} int nb_add_encryption */