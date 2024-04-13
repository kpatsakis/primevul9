connection_dirserv_flushed_some(dir_connection_t *conn)
{
  tor_assert(conn->base_.state == DIR_CONN_STATE_SERVER_WRITING);

  if (connection_get_outbuf_len(TO_CONN(conn)) >= DIRSERV_BUFFER_MIN)
    return 0;

  switch (conn->dir_spool_src) {
    case DIR_SPOOL_EXTRA_BY_DIGEST:
    case DIR_SPOOL_EXTRA_BY_FP:
    case DIR_SPOOL_SERVER_BY_DIGEST:
    case DIR_SPOOL_SERVER_BY_FP:
      return connection_dirserv_add_servers_to_outbuf(conn);
    case DIR_SPOOL_MICRODESC:
      return connection_dirserv_add_microdescs_to_outbuf(conn);
    case DIR_SPOOL_CACHED_DIR:
      return connection_dirserv_add_dir_bytes_to_outbuf(conn);
    case DIR_SPOOL_NETWORKSTATUS:
      return connection_dirserv_add_networkstatus_bytes_to_outbuf(conn);
    case DIR_SPOOL_NONE:
    default:
      return 0;
  }
}