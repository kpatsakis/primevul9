connection_dirserv_add_microdescs_to_outbuf(dir_connection_t *conn)
{
  microdesc_cache_t *cache = get_microdesc_cache();
  while (smartlist_len(conn->fingerprint_stack) &&
         connection_get_outbuf_len(TO_CONN(conn)) < DIRSERV_BUFFER_MIN) {
    char *fp256 = smartlist_pop_last(conn->fingerprint_stack);
    microdesc_t *md = microdesc_cache_lookup_by_digest256(cache, fp256);
    tor_free(fp256);
    if (!md || !md->body)
      continue;
    if (conn->zlib_state) {
      int last = !smartlist_len(conn->fingerprint_stack);
      connection_write_to_buf_zlib(md->body, md->bodylen, conn, last);
      if (last) {
        tor_zlib_free(conn->zlib_state);
        conn->zlib_state = NULL;
      }
    } else {
      connection_write_to_buf(md->body, md->bodylen, TO_CONN(conn));
    }
  }
  if (!smartlist_len(conn->fingerprint_stack)) {
    if (conn->zlib_state) {
      connection_write_to_buf_zlib("", 0, conn, 1);
      tor_zlib_free(conn->zlib_state);
      conn->zlib_state = NULL;
    }
    conn->dir_spool_src = DIR_SPOOL_NONE;
    smartlist_free(conn->fingerprint_stack);
    conn->fingerprint_stack = NULL;
  }
  return 0;
}