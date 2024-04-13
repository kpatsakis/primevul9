directory_remove_invalid(void)
{
  routerlist_t *rl = router_get_routerlist();
  smartlist_t *nodes = smartlist_new();
  smartlist_add_all(nodes, nodelist_get_list());

  SMARTLIST_FOREACH_BEGIN(nodes, node_t *, node) {
    const char *msg = NULL;
    routerinfo_t *ent = node->ri;
    char description[NODE_DESC_BUF_LEN];
    uint32_t r;
    if (!ent)
      continue;
    r = dirserv_router_get_status(ent, &msg, LOG_INFO);
    router_get_description(description, ent);
    if (r & FP_REJECT) {
      log_info(LD_DIRSERV, "Router %s is now rejected: %s",
               description, msg?msg:"");
      routerlist_remove(rl, ent, 0, time(NULL));
      continue;
    }
    if (bool_neq((r & FP_INVALID), !node->is_valid)) {
      log_info(LD_DIRSERV, "Router '%s' is now %svalid.", description,
               (r&FP_INVALID) ? "in" : "");
      node->is_valid = (r&FP_INVALID)?0:1;
    }
    if (bool_neq((r & FP_BADEXIT), node->is_bad_exit)) {
      log_info(LD_DIRSERV, "Router '%s' is now a %s exit", description,
               (r & FP_BADEXIT) ? "bad" : "good");
      node->is_bad_exit = (r&FP_BADEXIT) ? 1: 0;
    }
  } SMARTLIST_FOREACH_END(node);

  routerlist_assert_ok(rl);
  smartlist_free(nodes);
}