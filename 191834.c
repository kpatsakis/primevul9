dirserv_get_routerdesc_fingerprints(smartlist_t *fps_out, const char *key,
                                    const char **msg, int for_unencrypted_conn,
                                    int is_extrainfo)
{
  int by_id = 1;
  *msg = NULL;

  if (!strcmp(key, "all")) {
    routerlist_t *rl = router_get_routerlist();
    SMARTLIST_FOREACH(rl->routers, routerinfo_t *, r,
                      smartlist_add(fps_out,
                      tor_memdup(r->cache_info.identity_digest, DIGEST_LEN)));
    /* Treat "all" requests as if they were unencrypted */
    for_unencrypted_conn = 1;
  } else if (!strcmp(key, "authority")) {
    const routerinfo_t *ri = router_get_my_routerinfo();
    if (ri)
      smartlist_add(fps_out,
                    tor_memdup(ri->cache_info.identity_digest, DIGEST_LEN));
  } else if (!strcmpstart(key, "d/")) {
    by_id = 0;
    key += strlen("d/");
    dir_split_resource_into_fingerprints(key, fps_out, NULL,
                                         DSR_HEX|DSR_SORT_UNIQ);
  } else if (!strcmpstart(key, "fp/")) {
    key += strlen("fp/");
    dir_split_resource_into_fingerprints(key, fps_out, NULL,
                                         DSR_HEX|DSR_SORT_UNIQ);
  } else {
    *msg = "Key not recognized";
    return -1;
  }

  if (for_unencrypted_conn) {
    /* Remove anything that insists it not be sent unencrypted. */
    SMARTLIST_FOREACH_BEGIN(fps_out, char *, cp) {
        const signed_descriptor_t *sd;
        if (by_id)
          sd = get_signed_descriptor_by_fp(cp,is_extrainfo,0);
        else if (is_extrainfo)
          sd = extrainfo_get_by_descriptor_digest(cp);
        else
          sd = router_get_by_descriptor_digest(cp);
        if (sd && !sd->send_unencrypted) {
          tor_free(cp);
          SMARTLIST_DEL_CURRENT(fps_out, cp);
        }
    } SMARTLIST_FOREACH_END(cp);
  }

  if (!smartlist_len(fps_out)) {
    *msg = "Servers unavailable";
    return -1;
  }
  return 0;
}