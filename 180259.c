static int parse_part_encr_aes256(sockent_t *se, /* {{{ */
                                  void **ret_buffer, size_t *ret_buffer_size,
                                  int flags) {
  static int warning_has_been_printed = 0;

  char *buffer;
  size_t buffer_size;
  size_t buffer_offset;

  part_header_t ph;
  size_t ph_length;

  buffer = *ret_buffer;
  buffer_size = *ret_buffer_size;
  buffer_offset = 0;

  /* parse_packet assures this minimum size. */
  assert(buffer_size >= (sizeof(ph.type) + sizeof(ph.length)));

  BUFFER_READ(&ph.type, sizeof(ph.type));
  BUFFER_READ(&ph.length, sizeof(ph.length));
  ph_length = ntohs(ph.length);

  if ((ph_length <= PART_ENCRYPTION_AES256_SIZE) || (ph_length > buffer_size)) {
    ERROR("network plugin: AES-256 encrypted part "
          "with invalid length received.");
    return (-1);
  }

  if (warning_has_been_printed == 0) {
    WARNING("network plugin: Received encrypted packet, but the network "
            "plugin was not linked with libgcrypt, so I cannot "
            "decrypt it. The part will be discarded.");
    warning_has_been_printed = 1;
  }

  *ret_buffer = (void *)(((char *)*ret_buffer) + ph_length);
  *ret_buffer_size -= ph_length;

  return (0);
} /* }}} int parse_part_encr_aes256 */