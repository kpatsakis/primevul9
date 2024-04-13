static void network_send_buffer_encrypted(sockent_t *se, /* {{{ */
                                          const char *in_buffer,
                                          size_t in_buffer_size) {
  char buffer[BUFF_SIG_SIZE + in_buffer_size];
  size_t buffer_size;
  size_t buffer_offset;
  size_t header_size;
  size_t username_len;
  gcry_error_t err;
  gcry_cipher_hd_t cypher;

  /* Initialize the header fields */
  part_encryption_aes256_t pea = {.head.type = htons(TYPE_ENCR_AES256),
                                  .username = se->data.client.username};

  username_len = strlen(pea.username);
  if ((PART_ENCRYPTION_AES256_SIZE + username_len) > BUFF_SIG_SIZE) {
    ERROR("network plugin: Username too long: %s", pea.username);
    return;
  }

  buffer_size = PART_ENCRYPTION_AES256_SIZE + username_len + in_buffer_size;
  header_size = PART_ENCRYPTION_AES256_SIZE + username_len - sizeof(pea.hash);

  assert(buffer_size <= sizeof(buffer));
  DEBUG("network plugin: network_send_buffer_encrypted: "
        "buffer_size = %zu;",
        buffer_size);

  pea.head.length = htons(
      (uint16_t)(PART_ENCRYPTION_AES256_SIZE + username_len + in_buffer_size));
  pea.username_length = htons((uint16_t)username_len);

  /* Chose a random initialization vector. */
  gcry_randomize((void *)&pea.iv, sizeof(pea.iv), GCRY_STRONG_RANDOM);

  /* Create hash of the payload */
  gcry_md_hash_buffer(GCRY_MD_SHA1, pea.hash, in_buffer, in_buffer_size);

  /* Initialize the buffer */
  buffer_offset = 0;
  memset(buffer, 0, sizeof(buffer));

  BUFFER_ADD(&pea.head.type, sizeof(pea.head.type));
  BUFFER_ADD(&pea.head.length, sizeof(pea.head.length));
  BUFFER_ADD(&pea.username_length, sizeof(pea.username_length));
  BUFFER_ADD(pea.username, username_len);
  BUFFER_ADD(pea.iv, sizeof(pea.iv));
  assert(buffer_offset == header_size);
  BUFFER_ADD(pea.hash, sizeof(pea.hash));
  BUFFER_ADD(in_buffer, in_buffer_size);

  assert(buffer_offset == buffer_size);

  cypher = network_get_aes256_cypher(se, pea.iv, sizeof(pea.iv),
                                     se->data.client.password);
  if (cypher == NULL)
    return;

  /* Encrypt the buffer in-place */
  err = gcry_cipher_encrypt(cypher, buffer + header_size,
                            buffer_size - header_size,
                            /* in = */ NULL, /* in len = */ 0);
  if (err != 0) {
    ERROR("network plugin: gcry_cipher_encrypt returned: %s",
          gcry_strerror(err));
    return;
  }

  /* Send it out without further modifications */
  network_send_buffer_plain(se, buffer, buffer_size);
} /* }}} void network_send_buffer_encrypted */