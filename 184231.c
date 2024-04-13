concat_left_node_opt_info(OnigEncoding enc, OptNode* to, OptNode* add)
{
  int sb_reach, sm_reach;
  OptAnc tanc;

  concat_opt_anc_info(&tanc, &to->anc, &add->anc, to->len.max, add->len.max);
  copy_opt_anc_info(&to->anc, &tanc);

  if (add->sb.len > 0 && to->len.max == 0) {
    concat_opt_anc_info(&tanc, &to->anc, &add->sb.anc, to->len.max, add->len.max);
    copy_opt_anc_info(&add->sb.anc, &tanc);
  }

  if (add->map.value > 0 && to->len.max == 0) {
    if (add->map.mm.max == 0)
      add->map.anc.left |= to->anc.left;
  }

  sb_reach = to->sb.reach_end;
  sm_reach = to->sm.reach_end;

  if (add->len.max != 0)
    to->sb.reach_end = to->sm.reach_end = 0;

  if (add->sb.len > 0) {
    if (sb_reach) {
      concat_opt_exact(&to->sb, &add->sb, enc);
      clear_opt_exact(&add->sb);
    }
    else if (sm_reach) {
      concat_opt_exact(&to->sm, &add->sb, enc);
      clear_opt_exact(&add->sb);
    }
  }
  select_opt_exact(enc, &to->sm, &add->sb);
  select_opt_exact(enc, &to->sm, &add->sm);

  if (to->spr.len > 0) {
    if (add->len.max > 0) {
      if (to->spr.mm.max == 0)
        select_opt_exact(enc, &to->sb, &to->spr);
      else
        select_opt_exact(enc, &to->sm, &to->spr);
    }
  }
  else if (add->spr.len > 0) {
    copy_opt_exact(&to->spr, &add->spr);
  }

  select_opt_map(&to->map, &add->map);
  mml_add(&to->len, &add->len);
}