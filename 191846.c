connection_dirserv_add_dir_bytes_to_outbuf(dir_connection_t *conn)
{
  ssize_t bytes;
  int64_t remaining;

  bytes = DIRSERV_BUFFER_MIN - connection_get_outbuf_len(TO_CONN(conn));
  tor_assert(bytes > 0);
  tor_assert(conn->cached_dir);
  if (bytes < 8192)
    bytes = 8192;
  remaining = conn->cached_dir->dir_z_len - conn->cached_dir_offset;
  if (bytes > remaining)
    bytes = (ssize_t) remaining;

  if (conn->zlib_state) {
    connection_write_to_buf_zlib(
                             conn->cached_dir->dir_z + conn->cached_dir_offset,
                             bytes, conn, bytes == remaining);
  } else {
    connection_write_to_buf(conn->cached_dir->dir_z + conn->cached_dir_offset,
                            bytes, TO_CONN(conn));
  }
  conn->cached_dir_offset += bytes;
  if (conn->cached_dir_offset == (int)conn->cached_dir->dir_z_len) {
    /* We just wrote the last one; finish up. */
    connection_dirserv_finish_spooling(conn);
    cached_dir_decref(conn->cached_dir);
    conn->cached_dir = NULL;
  }
  return 0;
}