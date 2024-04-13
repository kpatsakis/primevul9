addr_policy_list_free(smartlist_t *lst)
{
  if (!lst) return;
  SMARTLIST_FOREACH(lst, addr_policy_t *, policy, addr_policy_free(policy));
  smartlist_free(lst);
}