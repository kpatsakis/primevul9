protocol_handshake_newstyle (void)
{
  GET_CONN;
  struct nbd_new_handshake handshake;
  uint16_t gflags;

  gflags = (NBD_FLAG_FIXED_NEWSTYLE | NBD_FLAG_NO_ZEROES) & mask_handshake;

  debug ("newstyle negotiation: flags: global 0x%x", gflags);

  handshake.nbdmagic = htobe64 (NBD_MAGIC);
  handshake.version = htobe64 (NBD_NEW_VERSION);
  handshake.gflags = htobe16 (gflags);

  if (conn->send (&handshake, sizeof handshake, 0) == -1) {
    nbdkit_error ("write: %s: %m", "sending newstyle handshake");
    return -1;
  }

  /* Client now sends us its 32 bit flags word ... */
  if (conn_recv_full (&conn->cflags, sizeof conn->cflags,
                      "reading initial client flags: conn->recv: %m") == -1)
    return -1;
  conn->cflags = be32toh (conn->cflags);
  /* ... which we check for accuracy. */
  debug ("newstyle negotiation: client flags: 0x%x", conn->cflags);
  if (conn->cflags & ~gflags) {
    nbdkit_error ("client requested unexpected flags 0x%x", conn->cflags);
    return -1;
  }

  /* Receive newstyle options. */
  if (negotiate_handshake_newstyle_options () == -1)
    return -1;

  return 0;
}