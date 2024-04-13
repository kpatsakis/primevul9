_compare_tor_version_str_ptr(const void **_a, const void **_b)
{
  const char *a = *_a, *b = *_b;
  int ca, cb;
  tor_version_t va, vb;
  ca = tor_version_parse(a, &va);
  cb = tor_version_parse(b, &vb);
  /* If they both parse, compare them. */
  if (!ca && !cb)
    return tor_version_compare(&va,&vb);
  /* If one parses, it comes first. */
  if (!ca && cb)
    return -1;
  if (ca && !cb)
    return 1;
  /* If neither parses, compare strings.  Also, the directory server admin
  ** needs to be smacked upside the head.  But Tor is tolerant and gentle. */
  return strcmp(a,b);
}