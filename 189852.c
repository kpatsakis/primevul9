static void flush_buffer(char *buf, size_t *len, CONNECTION *conn)
{
  buf[*len] = '\0';
  mutt_socket_write_n(conn, buf, *len);
  *len = 0;
}