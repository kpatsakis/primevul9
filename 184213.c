alt_merge_node_opt_info(OptNode* to, OptNode* add, OptEnv* env)
{
  alt_merge_opt_anc_info(&to->anc, &add->anc);
  alt_merge_opt_exact(&to->sb,  &add->sb, env);
  alt_merge_opt_exact(&to->sm,  &add->sm, env);
  alt_merge_opt_exact(&to->spr, &add->spr, env);
  alt_merge_opt_map(env->enc, &to->map, &add->map);

  mml_alt_merge(&to->len, &add->len);
}