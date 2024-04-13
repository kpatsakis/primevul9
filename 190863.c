update_regset_by_reg(OnigRegSet* set, regex_t* reg)
{
  if (set->n == 1) {
    set->enc          = reg->enc;
    set->anchor       = reg->anchor;
    set->anc_dmin     = reg->anc_dist_min;
    set->anc_dmax     = reg->anc_dist_max;
    set->all_low_high =
      (reg->optimize == OPTIMIZE_NONE || reg->dist_max == INFINITE_LEN) ? 0 : 1;
    set->anychar_inf  = (reg->anchor & ANCR_ANYCHAR_INF) != 0 ? 1 : 0;
  }
  else {
    int anchor;

    anchor = set->anchor & reg->anchor;
    if (anchor != 0) {
      OnigLen anc_dmin;
      OnigLen anc_dmax;

      anc_dmin = set->anc_dmin;
      anc_dmax = set->anc_dmax;
      if (anc_dmin > reg->anc_dist_min) anc_dmin = reg->anc_dist_min;
      if (anc_dmax < reg->anc_dist_max) anc_dmax = reg->anc_dist_max;
      set->anc_dmin = anc_dmin;
      set->anc_dmax = anc_dmax;
    }

    set->anchor = anchor;

    if (reg->optimize == OPTIMIZE_NONE || reg->dist_max == INFINITE_LEN)
      set->all_low_high = 0;

    if ((reg->anchor & ANCR_ANYCHAR_INF) != 0)
      set->anychar_inf = 1;
  }
}