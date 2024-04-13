dirserv_set_cached_consensus_networkstatus(const char *networkstatus,
                                           const char *flavor_name,
                                           const common_digests_t *digests,
                                           time_t published)
{
  cached_dir_t *new_networkstatus;
  cached_dir_t *old_networkstatus;
  if (!cached_consensuses)
    cached_consensuses = strmap_new();

  new_networkstatus = new_cached_dir(tor_strdup(networkstatus), published);
  memcpy(&new_networkstatus->digests, digests, sizeof(common_digests_t));
  old_networkstatus = strmap_set(cached_consensuses, flavor_name,
                                 new_networkstatus);
  if (old_networkstatus)
    cached_dir_decref(old_networkstatus);
}