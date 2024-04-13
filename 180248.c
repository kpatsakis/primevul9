static void network_send_buffer(char *buffer, size_t buffer_len) /* {{{ */
{
  DEBUG("network plugin: network_send_buffer: buffer_len = %zu", buffer_len);

  for (sockent_t *se = sending_sockets; se != NULL; se = se->next) {
#if HAVE_LIBGCRYPT
    if (se->data.client.security_level == SECURITY_LEVEL_ENCRYPT)
      network_send_buffer_encrypted(se, buffer, buffer_len);
    else if (se->data.client.security_level == SECURITY_LEVEL_SIGN)
      network_send_buffer_signed(se, buffer, buffer_len);
    else /* if (se->data.client.security_level == SECURITY_LEVEL_NONE) */
#endif   /* HAVE_LIBGCRYPT */
      network_send_buffer_plain(se, buffer, buffer_len);
  } /* for (sending_sockets) */
} /* }}} void network_send_buffer */