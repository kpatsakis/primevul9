set_bound_node_opt_info(OptNode* opt, MinMaxLen* plen)
{
  mml_copy(&(opt->sb.mm),  plen);
  mml_copy(&(opt->spr.mm), plen);
  mml_copy(&(opt->map.mm), plen);
}