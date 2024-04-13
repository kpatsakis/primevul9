connection_dirserv_add_networkstatus_bytes_to_outbuf(dir_connection_t *conn)
{

  while (connection_get_outbuf_len(TO_CONN(conn)) < DIRSERV_BUFFER_MIN) {
    if (conn->cached_dir) {
      int uncompressing = (conn->zlib_state != NULL);
      int r = connection_dirserv_add_dir_bytes_to_outbuf(conn);
      if (conn->dir_spool_src == DIR_SPOOL_NONE) {
        /* add_dir_bytes thinks we're done with the cached_dir.  But we
         * may have more cached_dirs! */
        conn->dir_spool_src = DIR_SPOOL_NETWORKSTATUS;
        /* This bit is tricky.  If we were uncompressing the last
         * networkstatus, we may need to make a new zlib object to
         * uncompress the next one. */
        if (uncompressing && ! conn->zlib_state &&
            conn->fingerprint_stack &&
            smartlist_len(conn->fingerprint_stack)) {
          conn->zlib_state = tor_zlib_new(0, ZLIB_METHOD, HIGH_COMPRESSION);
        }
      }
      if (r) return r;
    } else if (conn->fingerprint_stack &&
               smartlist_len(conn->fingerprint_stack)) {
      /* Add another networkstatus; start serving it. */
      char *fp = smartlist_pop_last(conn->fingerprint_stack);
      cached_dir_t *d = lookup_cached_dir_by_fp(fp);
      tor_free(fp);
      if (d) {
        ++d->refcnt;
        conn->cached_dir = d;
        conn->cached_dir_offset = 0;
      }
    } else {
      connection_dirserv_finish_spooling(conn);
      smartlist_free(conn->fingerprint_stack);
      conn->fingerprint_stack = NULL;
      return 0;
    }
  }
  return 0;
}