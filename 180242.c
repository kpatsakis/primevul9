static void network_send_buffer_signed(sockent_t *se, /* {{{ */
                                       const char *in_buffer,
                                       size_t in_buffer_size) {
  char buffer[BUFF_SIG_SIZE + in_buffer_size];
  size_t buffer_offset;
  size_t username_len;

  gcry_md_hd_t hd;
  gcry_error_t err;
  unsigned char *hash;

  hd = NULL;
  err = gcry_md_open(&hd, GCRY_MD_SHA256, GCRY_MD_FLAG_HMAC);
  if (err != 0) {
    ERROR("network plugin: Creating HMAC object failed: %s",
          gcry_strerror(err));
    return;
  }

  err = gcry_md_setkey(hd, se->data.client.password,
                       strlen(se->data.client.password));
  if (err != 0) {
    ERROR("network plugin: gcry_md_setkey failed: %s", gcry_strerror(err));
    gcry_md_close(hd);
    return;
  }

  username_len = strlen(se->data.client.username);
  if (username_len > (BUFF_SIG_SIZE - PART_SIGNATURE_SHA256_SIZE)) {
    ERROR("network plugin: Username too long: %s", se->data.client.username);
    return;
  }

  memcpy(buffer + PART_SIGNATURE_SHA256_SIZE, se->data.client.username,
         username_len);
  memcpy(buffer + PART_SIGNATURE_SHA256_SIZE + username_len, in_buffer,
         in_buffer_size);

  /* Initialize the `ps' structure. */
  part_signature_sha256_t ps = {
      .head.type = htons(TYPE_SIGN_SHA256),
      .head.length = htons(PART_SIGNATURE_SHA256_SIZE + username_len)};

  /* Calculate the hash value. */
  gcry_md_write(hd, buffer + PART_SIGNATURE_SHA256_SIZE,
                username_len + in_buffer_size);
  hash = gcry_md_read(hd, GCRY_MD_SHA256);
  if (hash == NULL) {
    ERROR("network plugin: gcry_md_read failed.");
    gcry_md_close(hd);
    return;
  }
  memcpy(ps.hash, hash, sizeof(ps.hash));

  /* Add the header */
  buffer_offset = 0;

  BUFFER_ADD(&ps.head.type, sizeof(ps.head.type));
  BUFFER_ADD(&ps.head.length, sizeof(ps.head.length));
  BUFFER_ADD(ps.hash, sizeof(ps.hash));

  assert(buffer_offset == PART_SIGNATURE_SHA256_SIZE);

  gcry_md_close(hd);
  hd = NULL;

  buffer_offset = PART_SIGNATURE_SHA256_SIZE + username_len + in_buffer_size;
  network_send_buffer_plain(se, buffer, buffer_offset);
} /* }}} void network_send_buffer_signed */