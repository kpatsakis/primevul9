parse_addr_policy(config_line_t *cfg, smartlist_t **dest,
                  int assume_action)
{
  smartlist_t *result;
  smartlist_t *entries;
  addr_policy_t *item;
  int r = 0;

  if (!cfg)
    return 0;

  result = smartlist_create();
  entries = smartlist_create();
  for (; cfg; cfg = cfg->next) {
    smartlist_split_string(entries, cfg->value, ",",
                           SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
    SMARTLIST_FOREACH(entries, const char *, ent,
    {
      log_debug(LD_CONFIG,"Adding new entry '%s'",ent);
      item = router_parse_addr_policy_item_from_string(ent, assume_action);
      if (item) {
        smartlist_add(result, item);
      } else {
        log_warn(LD_CONFIG,"Malformed policy '%s'.", ent);
        r = -1;
      }
    });
    SMARTLIST_FOREACH(entries, char *, ent, tor_free(ent));
    smartlist_clear(entries);
  }
  smartlist_free(entries);
  if (r == -1) {
    addr_policy_list_free(result);
  } else {
    policy_expand_private(&result);

    if (*dest) {
      smartlist_add_all(*dest, result);
      smartlist_free(result);
    } else {
      *dest = result;
    }
  }

  return r;
}