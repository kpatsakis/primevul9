dump_distinct_digest_count(int severity)
{
#ifdef COUNT_DISTINCT_DIGESTS
  if (!verified_digests)
    verified_digests = digestmap_new();
  log(severity, LD_GENERAL, "%d *distinct* router digests verified",
      digestmap_size(verified_digests));
#else
  (void)severity; /* suppress "unused parameter" warning */
#endif
}