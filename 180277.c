static int parse_part_string(void **ret_buffer, size_t *ret_buffer_len,
                             char *output, size_t const output_len) {
  char *buffer = *ret_buffer;
  size_t buffer_len = *ret_buffer_len;

  uint16_t tmp16;
  size_t const header_size = 2 * sizeof(uint16_t);

  uint16_t pkg_length;
  size_t payload_size;

  if (output_len == 0)
    return (EINVAL);

  if (buffer_len < header_size) {
    WARNING("network plugin: parse_part_string: "
            "Packet too short: "
            "Chunk of at least size %zu expected, "
            "but buffer has only %zu bytes left.",
            header_size, buffer_len);
    return (-1);
  }

  memcpy((void *)&tmp16, buffer, sizeof(tmp16));
  buffer += sizeof(tmp16);
  /* pkg_type = ntohs (tmp16); */

  memcpy((void *)&tmp16, buffer, sizeof(tmp16));
  buffer += sizeof(tmp16);
  pkg_length = ntohs(tmp16);
  payload_size = ((size_t)pkg_length) - header_size;

  /* Check that packet fits in the input buffer */
  if (pkg_length > buffer_len) {
    WARNING("network plugin: parse_part_string: "
            "Packet too big: "
            "Chunk of size %" PRIu16 " received, "
            "but buffer has only %zu bytes left.",
            pkg_length, buffer_len);
    return (-1);
  }

  /* Check that pkg_length is in the valid range */
  if (pkg_length <= header_size) {
    WARNING("network plugin: parse_part_string: "
            "Packet too short: "
            "Header claims this packet is only %hu "
            "bytes long.",
            pkg_length);
    return (-1);
  }

  /* Check that the package data fits into the output buffer.
   * The previous if-statement ensures that:
   * `pkg_length > header_size' */
  if (output_len < payload_size) {
    WARNING("network plugin: parse_part_string: "
            "Buffer too small: "
            "Output buffer holds %zu bytes, "
            "which is too small to hold the received "
            "%zu byte string.",
            output_len, payload_size);
    return (-1);
  }

  /* All sanity checks successfull, let's copy the data over */
  memcpy((void *)output, (void *)buffer, payload_size);
  buffer += payload_size;

  /* For some very weird reason '\0' doesn't do the trick on SPARC in
   * this statement. */
  if (output[payload_size - 1] != 0) {
    WARNING("network plugin: parse_part_string: "
            "Received string does not end "
            "with a NULL-byte.");
    return (-1);
  }

  *ret_buffer = buffer;
  *ret_buffer_len = buffer_len - pkg_length;

  return (0);
} /* int parse_part_string */