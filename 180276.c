static void network_init_buffer(void) {
  memset(send_buffer, 0, network_config_packet_size);
  send_buffer_ptr = send_buffer;
  send_buffer_fill = 0;
  send_buffer_last_update = 0;

  memset(&send_buffer_vl, 0, sizeof(send_buffer_vl));
} /* int network_init_buffer */