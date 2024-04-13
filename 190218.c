policies_parse_exit_policy(config_line_t *cfg, smartlist_t **dest,
                           int rejectprivate, const char *local_address,
                           int add_default_policy)
{
  if (rejectprivate) {
    append_exit_policy_string(dest, "reject private:*");
    if (local_address) {
      char buf[POLICY_BUF_LEN];
      tor_snprintf(buf, sizeof(buf), "reject %s:*", local_address);
      append_exit_policy_string(dest, buf);
    }
  }
  if (parse_addr_policy(cfg, dest, -1))
    return -1;
  if (add_default_policy)
    append_exit_policy_string(dest, DEFAULT_EXIT_POLICY);
  else
    append_exit_policy_string(dest, "reject *:*");
  exit_policy_remove_redundancies(*dest);

  return 0;
}