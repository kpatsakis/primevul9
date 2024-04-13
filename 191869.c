get_signed_descriptor_by_fp(const char *fp, int extrainfo,
                            time_t publish_cutoff)
{
  if (router_digest_is_me(fp)) {
    if (extrainfo)
      return &(router_get_my_extrainfo()->cache_info);
    else
      return &(router_get_my_routerinfo()->cache_info);
  } else {
    const routerinfo_t *ri = router_get_by_id_digest(fp);
    if (ri &&
        ri->cache_info.published_on > publish_cutoff) {
      if (extrainfo)
        return extrainfo_get_by_descriptor_digest(
                                     ri->cache_info.extra_info_digest);
      else
        return &ri->cache_info;
    }
  }
  return NULL;
}