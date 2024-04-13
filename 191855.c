connection_dirserv_add_servers_to_outbuf(dir_connection_t *conn)
{
  int by_fp = (conn->dir_spool_src == DIR_SPOOL_SERVER_BY_FP ||
               conn->dir_spool_src == DIR_SPOOL_EXTRA_BY_FP);
  int extra = (conn->dir_spool_src == DIR_SPOOL_EXTRA_BY_FP ||
               conn->dir_spool_src == DIR_SPOOL_EXTRA_BY_DIGEST);
  time_t publish_cutoff = time(NULL)-ROUTER_MAX_AGE_TO_PUBLISH;

  const or_options_t *options = get_options();

  while (smartlist_len(conn->fingerprint_stack) &&
         connection_get_outbuf_len(TO_CONN(conn)) < DIRSERV_BUFFER_MIN) {
    const char *body;
    char *fp = smartlist_pop_last(conn->fingerprint_stack);
    const signed_descriptor_t *sd = NULL;
    if (by_fp) {
      sd = get_signed_descriptor_by_fp(fp, extra, publish_cutoff);
    } else {
      sd = extra ? extrainfo_get_by_descriptor_digest(fp)
        : router_get_by_descriptor_digest(fp);
    }
    tor_free(fp);
    if (!sd)
      continue;
    if (!connection_dir_is_encrypted(conn) && !sd->send_unencrypted) {
      /* we did this check once before (so we could have an accurate size
       * estimate and maybe send a 404 if somebody asked for only bridges on a
       * connection), but we need to do it again in case a previously
       * unknown bridge descriptor has shown up between then and now. */
      continue;
    }

    /** If we are the bridge authority and the descriptor is a bridge
     * descriptor, remember that we served this descriptor for desc stats. */
    if (options->BridgeAuthoritativeDir && by_fp) {
      const routerinfo_t *router =
          router_get_by_id_digest(sd->identity_digest);
      /* router can be NULL here when the bridge auth is asked for its own
       * descriptor. */
      if (router && router->purpose == ROUTER_PURPOSE_BRIDGE)
        rep_hist_note_desc_served(sd->identity_digest);
    }
    body = signed_descriptor_get_body(sd);
    if (conn->zlib_state) {
      int last = ! smartlist_len(conn->fingerprint_stack);
      connection_write_to_buf_zlib(body, sd->signed_descriptor_len, conn,
                                   last);
      if (last) {
        tor_zlib_free(conn->zlib_state);
        conn->zlib_state = NULL;
      }
    } else {
      connection_write_to_buf(body,
                              sd->signed_descriptor_len,
                              TO_CONN(conn));
    }
  }

  if (!smartlist_len(conn->fingerprint_stack)) {
    /* We just wrote the last one; finish up. */
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