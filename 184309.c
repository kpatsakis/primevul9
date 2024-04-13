set_optimize_info_from_tree(Node* node, regex_t* reg, ScanEnv* scan_env)
{
  int r;
  OptNode opt;
  OptEnv env;

  env.enc            = reg->enc;
  env.case_fold_flag = reg->case_fold_flag;
  env.scan_env       = scan_env;
  mml_clear(&env.mm);

  r = optimize_nodes(node, &opt, &env);
  if (r != 0) return r;

  reg->anchor = opt.anc.left & (ANCR_BEGIN_BUF |
        ANCR_BEGIN_POSITION | ANCR_ANYCHAR_INF | ANCR_ANYCHAR_INF_ML |
        ANCR_LOOK_BEHIND);

  if ((opt.anc.left & (ANCR_LOOK_BEHIND | ANCR_PREC_READ_NOT)) != 0)
    reg->anchor &= ~ANCR_ANYCHAR_INF_ML;

  reg->anchor |= opt.anc.right & (ANCR_END_BUF | ANCR_SEMI_END_BUF |
                                  ANCR_PREC_READ_NOT);

  if (reg->anchor & (ANCR_END_BUF | ANCR_SEMI_END_BUF)) {
    reg->anc_dist_min = opt.len.min;
    reg->anc_dist_max = opt.len.max;
  }

  if (opt.sb.len > 0 || opt.sm.len > 0) {
    select_opt_exact(reg->enc, &opt.sb, &opt.sm);
    if (opt.map.value > 0 && comp_opt_exact_or_map(&opt.sb, &opt.map) > 0) {
      goto set_map;
    }
    else {
      r = set_optimize_exact(reg, &opt.sb);
      set_sub_anchor(reg, &opt.sb.anc);
    }
  }
  else if (opt.map.value > 0) {
  set_map:
    set_optimize_map(reg, &opt.map);
    set_sub_anchor(reg, &opt.map.anc);
  }
  else {
    reg->sub_anchor |= opt.anc.left & ANCR_BEGIN_LINE;
    if (opt.len.max == 0)
      reg->sub_anchor |= opt.anc.right & ANCR_END_LINE;
  }

#if defined(ONIG_DEBUG_COMPILE) || defined(ONIG_DEBUG_MATCH)
  print_optimize_info(DBGFP, reg);
#endif
  return r;
}