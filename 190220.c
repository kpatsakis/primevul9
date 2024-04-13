exit_policy_is_general_exit(smartlist_t *policy)
{
  static const int ports[] = { 80, 443, 6667 };
  int n_allowed = 0;
  int i;
  if (!policy) /*XXXX disallow NULL policies? */
    return 0;

  for (i = 0; i < 3; ++i) {
    n_allowed += exit_policy_is_general_exit_helper(policy, ports[i]);
  }
  return n_allowed >= 2;
}