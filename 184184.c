clear_node_opt_info(OptNode* opt)
{
  mml_clear(&opt->len);
  clear_opt_anc_info(&opt->anc);
  clear_opt_exact(&opt->sb);
  clear_opt_exact(&opt->sm);
  clear_opt_exact(&opt->spr);
  clear_opt_map(&opt->map);
}