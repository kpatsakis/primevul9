dirserv_get_routerdescs(smartlist_t *descs_out, const char *key,
                        const char **msg)
{
  *msg = NULL;

  if (!strcmp(key, "/tor/server/all")) {
    routerlist_t *rl = router_get_routerlist();
    SMARTLIST_FOREACH(rl->routers, routerinfo_t *, r,
                      smartlist_add(descs_out, &(r->cache_info)));
  } else if (!strcmp(key, "/tor/server/authority")) {
    const routerinfo_t *ri = router_get_my_routerinfo();
    if (ri)
      smartlist_add(descs_out, (void*) &(ri->cache_info));
  } else if (!strcmpstart(key, "/tor/server/d/")) {
    smartlist_t *digests = smartlist_new();
    key += strlen("/tor/server/d/");
    dir_split_resource_into_fingerprints(key, digests, NULL,
                                         DSR_HEX|DSR_SORT_UNIQ);
    SMARTLIST_FOREACH(digests, const char *, d,
       {
         signed_descriptor_t *sd = router_get_by_descriptor_digest(d);
         if (sd)
           smartlist_add(descs_out,sd);
       });
    SMARTLIST_FOREACH(digests, char *, d, tor_free(d));
    smartlist_free(digests);
  } else if (!strcmpstart(key, "/tor/server/fp/")) {
    smartlist_t *digests = smartlist_new();
    time_t cutoff = time(NULL) - ROUTER_MAX_AGE_TO_PUBLISH;
    key += strlen("/tor/server/fp/");
    dir_split_resource_into_fingerprints(key, digests, NULL,
                                         DSR_HEX|DSR_SORT_UNIQ);
    SMARTLIST_FOREACH_BEGIN(digests, const char *, d) {
         if (router_digest_is_me(d)) {
           /* calling router_get_my_routerinfo() to make sure it exists */
           const routerinfo_t *ri = router_get_my_routerinfo();
           if (ri)
             smartlist_add(descs_out, (void*) &(ri->cache_info));
         } else {
           const routerinfo_t *ri = router_get_by_id_digest(d);
           /* Don't actually serve a descriptor that everyone will think is
            * expired.  This is an (ugly) workaround to keep buggy 0.1.1.10
            * Tors from downloading descriptors that they will throw away.
            */
           if (ri && ri->cache_info.published_on > cutoff)
             smartlist_add(descs_out, (void*) &(ri->cache_info));
         }
    } SMARTLIST_FOREACH_END(d);
    SMARTLIST_FOREACH(digests, char *, d, tor_free(d));
    smartlist_free(digests);
  } else {
    *msg = "Key not recognized";
    return -1;
  }

  if (!smartlist_len(descs_out)) {
    *msg = "Servers unavailable";
    return -1;
  }
  return 0;
}