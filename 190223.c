policies_exit_policy_append_reject_star(smartlist_t **dest)
{
  append_exit_policy_string(dest, "reject *:*");
}