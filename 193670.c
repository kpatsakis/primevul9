set_optimize_exact(regex_t* reg, OptExact* e)
{
  int r;

  if (e->len == 0) return 0;

  if (e->ignore_case) {
    reg->exact = (UChar* )xmalloc(e->len);
    CHECK_NULL_RETURN_MEMERR(reg->exact);
    xmemcpy(reg->exact, e->s, e->len);
    reg->exact_end = reg->exact + e->len;
    reg->optimize = OPTIMIZE_EXACT_IC;
  }
  else {
    int allow_reverse;

    reg->exact = onigenc_strdup(reg->enc, e->s, e->s + e->len);
    CHECK_NULL_RETURN_MEMERR(reg->exact);
    reg->exact_end = reg->exact + e->len;
 
    allow_reverse =
      ONIGENC_IS_ALLOWED_REVERSE_MATCH(reg->enc, reg->exact, reg->exact_end);

    if (e->len >= 3 || (e->len >= 2 && allow_reverse)) {
      r = set_bm_skip(reg->exact, reg->exact_end, reg->enc,
                      reg->map, &(reg->int_map));
      if (r != 0) return r;

      reg->optimize = (allow_reverse != 0
                       ? OPTIMIZE_EXACT_BM : OPTIMIZE_EXACT_BM_NO_REV);
    }
    else {
      reg->optimize = OPTIMIZE_EXACT;
    }
  }

  reg->dmin = e->mmd.min;
  reg->dmax = e->mmd.max;

  if (reg->dmin != INFINITE_LEN) {
    reg->threshold_len = reg->dmin + (int )(reg->exact_end - reg->exact);
  }

  return 0;
}