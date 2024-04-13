append_exit_policy_string(smartlist_t **policy, const char *more)
{
  config_line_t tmp;

  tmp.key = NULL;
  tmp.value = (char*) more;
  tmp.next = NULL;
  if (parse_addr_policy(&tmp, policy, -1)<0) {
    log_warn(LD_BUG, "Unable to parse internally generated policy %s",more);
  }
}