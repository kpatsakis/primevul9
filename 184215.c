set_optimize_exact(regex_t* reg, OptStr* e)
{
  int r;
  int allow_reverse;

  if (e->len == 0) return 0;

  reg->exact = (UChar* )xmalloc(e->len);
  CHECK_NULL_RETURN_MEMERR(reg->exact);
  xmemcpy(reg->exact, e->s, e->len);
  reg->exact_end = reg->exact + e->len;

  allow_reverse =
    ONIGENC_IS_ALLOWED_REVERSE_MATCH(reg->enc, reg->exact, reg->exact_end);

  if (e->len >= 2 || (e->len >= 1 && allow_reverse)) {
    r = set_sunday_quick_search_or_bmh_skip_table(reg, 0,
                                                  reg->exact, reg->exact_end,
                                                  reg->map, &(reg->map_offset));
    if (r != 0) return r;

    reg->optimize = (allow_reverse != 0
                     ? OPTIMIZE_STR_FAST
                     : OPTIMIZE_STR_FAST_STEP_FORWARD);
  }
  else {
    reg->optimize = OPTIMIZE_STR;
  }

  reg->dist_min = e->mm.min;
  reg->dist_max = e->mm.max;

  if (reg->dist_min != INFINITE_LEN) {
    int n = (int )(reg->exact_end - reg->exact);
    reg->threshold_len = reg->dist_min + n;
  }

  return 0;
}