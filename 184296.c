set_sunday_quick_search_or_bmh_skip_table(regex_t* reg, int case_expand,
                                          UChar* s, UChar* end,
                                          UChar skip[], int* roffset)
{
  int i, j, k, len, offset;
  int n, clen;
  UChar* p;
  OnigEncoding enc;
  OnigCaseFoldCodeItem items[ONIGENC_GET_CASE_FOLD_CODES_MAX_NUM];
  UChar buf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

  enc = reg->enc;
  offset = ENC_GET_SKIP_OFFSET(enc);
  if (offset == ENC_SKIP_OFFSET_1_OR_0) {
    UChar* p = s;
    while (1) {
      len = enclen(enc, p);
      if (p + len >= end) {
        if (len == 1) offset = 1;
        else          offset = 0;
        break;
      }
      p += len;
    }
  }

  len = (int )(end - s);
  if (len + offset >= UCHAR_MAX)
    return ONIGERR_PARSER_BUG;

  *roffset = offset;

  for (i = 0; i < CHAR_MAP_SIZE; i++) {
    skip[i] = (UChar )(len + offset);
  }

  for (p = s; p < end; ) {
    int z;

    clen = enclen(enc, p);
    if (p + clen > end) clen = (int )(end - p);

    len = (int )(end - p);
    for (j = 0; j < clen; j++) {
      z = len - j + (offset - 1);
      if (z <= 0) break;
      skip[p[j]] = z;
    }

    if (case_expand != 0) {
      n = ONIGENC_GET_CASE_FOLD_CODES_BY_STR(enc, reg->case_fold_flag,
                                             p, end, items);
      for (k = 0; k < n; k++) {
        ONIGENC_CODE_TO_MBC(enc, items[k].code[0], buf);
        for (j = 0; j < clen; j++) {
          z = len - j + (offset - 1);
          if (z <= 0) break;
          if (skip[buf[j]] > z)
            skip[buf[j]] = z;
        }
      }
    }

    p += clen;
  }

  return 0;
}