policies_free_all(void)
{
  addr_policy_list_free(reachable_or_addr_policy);
  reachable_or_addr_policy = NULL;
  addr_policy_list_free(reachable_dir_addr_policy);
  reachable_dir_addr_policy = NULL;
  addr_policy_list_free(socks_policy);
  socks_policy = NULL;
  addr_policy_list_free(dir_policy);
  dir_policy = NULL;
  addr_policy_list_free(authdir_reject_policy);
  authdir_reject_policy = NULL;
  addr_policy_list_free(authdir_invalid_policy);
  authdir_invalid_policy = NULL;
  addr_policy_list_free(authdir_baddir_policy);
  authdir_baddir_policy = NULL;
  addr_policy_list_free(authdir_badexit_policy);
  authdir_badexit_policy = NULL;

  if (!HT_EMPTY(&policy_root)) {
    policy_map_ent_t **ent;
    int n = 0;
    char buf[POLICY_BUF_LEN];

    log_warn(LD_MM, "Still had %d address policies cached at shutdown.",
             (int)HT_SIZE(&policy_root));

    /* Note the first 10 cached policies to try to figure out where they
     * might be coming from. */
    HT_FOREACH(ent, policy_map, &policy_root) {
      if (++n > 10)
        break;
      if (policy_write_item(buf, sizeof(buf), (*ent)->policy, 0) >= 0)
        log_warn(LD_MM,"  %d [%d]: %s", n, (*ent)->policy->refcnt, buf);
    }
  }
  HT_CLEAR(policy_map, &policy_root);
}