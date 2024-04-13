dirserv_have_any_serverdesc(smartlist_t *fps, int spool_src)
{
  time_t publish_cutoff = time(NULL)-ROUTER_MAX_AGE_TO_PUBLISH;
  SMARTLIST_FOREACH_BEGIN(fps, const char *, fp) {
      switch (spool_src)
      {
        case DIR_SPOOL_EXTRA_BY_DIGEST:
          if (extrainfo_get_by_descriptor_digest(fp)) return 1;
          break;
        case DIR_SPOOL_SERVER_BY_DIGEST:
          if (router_get_by_descriptor_digest(fp)) return 1;
          break;
        case DIR_SPOOL_EXTRA_BY_FP:
        case DIR_SPOOL_SERVER_BY_FP:
          if (get_signed_descriptor_by_fp(fp,
                spool_src == DIR_SPOOL_EXTRA_BY_FP, publish_cutoff))
            return 1;
          break;
      }
  } SMARTLIST_FOREACH_END(fp);
  return 0;
}