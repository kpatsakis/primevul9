alt_merge_opt_map(OnigEncoding enc, OptMap* to, OptMap* add)
{
  int i, val;

  /* if (! mml_is_equal(&to->mm, &add->mm)) return ; */
  if (to->value == 0) return ;
  if (add->value == 0 || to->mm.max < add->mm.min) {
    clear_opt_map(to);
    return ;
  }

  mml_alt_merge(&to->mm, &add->mm);

  val = 0;
  for (i = 0; i < CHAR_MAP_SIZE; i++) {
    if (add->map[i])
      to->map[i] = 1;

    if (to->map[i])
      val += map_position_value(enc, i);
  }
  to->value = val;

  alt_merge_opt_anc_info(&to->anc, &add->anc);
}