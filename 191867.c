lookup_cached_dir_by_fp(const char *fp)
{
  cached_dir_t *d = NULL;
  if (tor_digest_is_zero(fp) && cached_consensuses) {
    d = strmap_get(cached_consensuses, "ns");
  } else if (memchr(fp, '\0', DIGEST_LEN) && cached_consensuses &&
           (d = strmap_get(cached_consensuses, fp))) {
    /* this here interface is a nasty hack XXXX */;
  }
  return d;
}