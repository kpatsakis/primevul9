conn_recv_full (void *buf, size_t len, const char *fmt, ...)
{
  GET_CONN;
  int r = conn->recv (buf, len);
  va_list args;

  if (r == -1) {
    va_start (args, fmt);
    nbdkit_verror (fmt, args);
    va_end (args);
    return -1;
  }
  if (r == 0) {
    /* During negotiation, client EOF on message boundary is less
     * severe than failure in the middle of the buffer. */
    debug ("client closed input socket, closing connection");
    return -1;
  }
  return r;
}