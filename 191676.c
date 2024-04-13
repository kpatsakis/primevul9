static int parse_part_encr_aes256 (sockent_t *se, /* {{{ */
		void **ret_buffer, size_t *ret_buffer_len,
		int flags)
{
  char  *buffer = *ret_buffer;
  size_t buffer_len = *ret_buffer_len;
  size_t payload_len;
  size_t part_size;
  size_t buffer_offset;
  uint16_t username_len;
  part_encryption_aes256_t pea;
  unsigned char hash[sizeof (pea.hash)];

  gcry_cipher_hd_t cypher;
  gcry_error_t err;

  /* Make sure at least the header if available. */
  if (buffer_len <= PART_ENCRYPTION_AES256_SIZE)
  {
    NOTICE ("network plugin: parse_part_encr_aes256: "
        "Discarding short packet.");
    return (-1);
  }

  buffer_offset = 0;

  /* Copy the unencrypted information into `pea'. */
  BUFFER_READ (&pea.head.type, sizeof (pea.head.type));
  BUFFER_READ (&pea.head.length, sizeof (pea.head.length));

  /* Check the `part size'. */
  part_size = ntohs (pea.head.length);
  if ((part_size <= PART_ENCRYPTION_AES256_SIZE)
      || (part_size > buffer_len))
  {
    NOTICE ("network plugin: parse_part_encr_aes256: "
        "Discarding part with invalid size.");
    return (-1);
  }

  /* Read the username */
  BUFFER_READ (&username_len, sizeof (username_len));
  username_len = ntohs (username_len);

  if ((username_len <= 0)
      || (username_len > (part_size - (PART_ENCRYPTION_AES256_SIZE + 1))))
  {
    NOTICE ("network plugin: parse_part_encr_aes256: "
        "Discarding part with invalid username length.");
    return (-1);
  }

  assert (username_len > 0);
  pea.username = malloc (username_len + 1);
  if (pea.username == NULL)
    return (-ENOMEM);
  BUFFER_READ (pea.username, username_len);
  pea.username[username_len] = 0;

  /* Last but not least, the initialization vector */
  BUFFER_READ (pea.iv, sizeof (pea.iv));

  /* Make sure we are at the right position */
  assert (buffer_offset == (username_len +
        PART_ENCRYPTION_AES256_SIZE - sizeof (pea.hash)));

  cypher = network_get_aes256_cypher (se, pea.iv, sizeof (pea.iv),
      pea.username);
  if (cypher == NULL)
  {
    sfree (pea.username);
    return (-1);
  }

  payload_len = part_size - (PART_ENCRYPTION_AES256_SIZE + username_len);
  assert (payload_len > 0);

  /* Decrypt the packet in-place */
  err = gcry_cipher_decrypt (cypher,
      buffer    + buffer_offset,
      part_size - buffer_offset,
      /* in = */ NULL, /* in len = */ 0);
  if (err != 0)
  {
    sfree (pea.username);
    ERROR ("network plugin: gcry_cipher_decrypt returned: %s",
        gcry_strerror (err));
    return (-1);
  }

  /* Read the hash */
  BUFFER_READ (pea.hash, sizeof (pea.hash));

  /* Make sure we're at the right position - again */
  assert (buffer_offset == (username_len + PART_ENCRYPTION_AES256_SIZE));
  assert (buffer_offset == (part_size - payload_len));

  /* Check hash sum */
  memset (hash, 0, sizeof (hash));
  gcry_md_hash_buffer (GCRY_MD_SHA1, hash,
      buffer + buffer_offset, payload_len);
  if (memcmp (hash, pea.hash, sizeof (hash)) != 0)
  {
    sfree (pea.username);
    ERROR ("network plugin: Decryption failed: Checksum mismatch.");
    return (-1);
  }

  parse_packet (se, buffer + buffer_offset, payload_len,
      flags | PP_ENCRYPTED, pea.username);

  /* XXX: Free pea.username?!? */

  /* Update return values */
  *ret_buffer =     buffer     + part_size;
  *ret_buffer_len = buffer_len - part_size;

  sfree (pea.username);

  return (0);
} /* }}} int parse_part_encr_aes256 */