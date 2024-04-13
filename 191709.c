static int parse_part_sign_sha256 (sockent_t *se, /* {{{ */
    void **ret_buffer, size_t *ret_buffer_size, int flags)
{
  static int warning_has_been_printed = 0;

  char *buffer;
  size_t buffer_size;
  size_t buffer_offset;
  uint16_t part_len;

  part_signature_sha256_t pss;

  buffer = *ret_buffer;
  buffer_size = *ret_buffer_size;
  buffer_offset = 0;

  if (buffer_size <= PART_SIGNATURE_SHA256_SIZE)
    return (-ENOMEM);

  BUFFER_READ (&pss.head.type, sizeof (pss.head.type));
  BUFFER_READ (&pss.head.length, sizeof (pss.head.length));
  part_len = ntohs (pss.head.length);

  if ((part_len <= PART_SIGNATURE_SHA256_SIZE)
      || (part_len > buffer_size))
    return (-EINVAL);

  if (warning_has_been_printed == 0)
  {
    WARNING ("network plugin: Received signed packet, but the network "
        "plugin was not linked with libgcrypt, so I cannot "
        "verify the signature. The packet will be accepted.");
    warning_has_been_printed = 1;
  }

  parse_packet (se, buffer + part_len, buffer_size - part_len, flags,
      /* username = */ NULL);

  *ret_buffer = buffer + buffer_size;
  *ret_buffer_size = 0;

  return (0);
} /* }}} int parse_part_sign_sha256 */