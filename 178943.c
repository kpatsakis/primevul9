policies_set_router_exitpolicy_to_reject_all(routerinfo_t *r)
{
  addr_policy_t *item;
  addr_policy_list_free(r->exit_policy);
  r->exit_policy = smartlist_create();
  item = router_parse_addr_policy_item_from_string("reject *:*", -1);
  smartlist_add(r->exit_policy, item);
}