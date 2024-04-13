gnutls_handshake_set_max_packet_length (gnutls_session_t session, size_t max)
{
  session->internals.max_handshake_data_buffer_size = max;
}