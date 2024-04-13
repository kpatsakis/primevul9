add_ctype_to_cc_by_range(CClassNode* cc, int ctype ARG_UNUSED, int not,
			 OnigEncoding enc ARG_UNUSED,
                         OnigCodePoint sb_out, const OnigCodePoint mbr[])
{
  int i, r;
  OnigCodePoint j;

  int n = ONIGENC_CODE_RANGE_NUM(mbr);

  if (not == 0) {
    for (i = 0; i < n; i++) {
      for (j  = ONIGENC_CODE_RANGE_FROM(mbr, i);
           j <= ONIGENC_CODE_RANGE_TO(mbr, i); j++) {
        if (j >= sb_out) {
          if (j > ONIGENC_CODE_RANGE_FROM(mbr, i)) {
            r = add_code_range_to_buf(&(cc->mbuf), j,
                                      ONIGENC_CODE_RANGE_TO(mbr, i));
            if (r != 0) return r;
            i++;
          }

          goto sb_end;
        }
        BITSET_SET_BIT(cc->bs, j);
      }
    }

  sb_end:
    for ( ; i < n; i++) {
      r = add_code_range_to_buf(&(cc->mbuf),
                                ONIGENC_CODE_RANGE_FROM(mbr, i),
                                ONIGENC_CODE_RANGE_TO(mbr, i));
      if (r != 0) return r;
    }
  }
  else {
    OnigCodePoint prev = 0;

    for (i = 0; i < n; i++) {
      for (j = prev; j < ONIGENC_CODE_RANGE_FROM(mbr, i); j++) {
        if (j >= sb_out) {
          goto sb_end2;
        }
        BITSET_SET_BIT(cc->bs, j);
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i) + 1;
    }
    for (j = prev; j < sb_out; j++) {
      BITSET_SET_BIT(cc->bs, j);
    }

  sb_end2:
    prev = sb_out;

    for (i = 0; i < n; i++) {
      if (prev < ONIGENC_CODE_RANGE_FROM(mbr, i)) {
        r = add_code_range_to_buf(&(cc->mbuf), prev,
                                  ONIGENC_CODE_RANGE_FROM(mbr, i) - 1);
        if (r != 0) return r;
      }
      prev = ONIGENC_CODE_RANGE_TO(mbr, i) + 1;
    }
    if (prev < 0x7fffffff) {
      r = add_code_range_to_buf(&(cc->mbuf), prev, 0x7fffffff);
      if (r != 0) return r;
    }
  }

  return 0;
}