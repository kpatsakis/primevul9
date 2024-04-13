parse_callout_args(int skip_mode, int cterm, UChar** src, UChar* end,
                   unsigned int types[], OnigValue vals[], ScanEnv* env)
{
#define MAX_CALLOUT_ARG_BYTE_LENGTH   128

  int r;
  int n;
  int esc;
  int cn;
  UChar* s;
  UChar* e;
  UChar* eesc;
  OnigCodePoint c;
  UChar* bufend;
  UChar buf[MAX_CALLOUT_ARG_BYTE_LENGTH];
  OnigEncoding enc = env->enc;
  UChar* p = *src;

  if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

  n = 0;
  while (n < ONIG_CALLOUT_MAX_ARGS_NUM) {
    c   = 0;
    cn  = 0;
    esc = 0;
    eesc = 0;
    bufend = buf;
    s = e = p;
    while (1) {
      if (PEND) return ONIGERR_INVALID_CALLOUT_PATTERN;

      e = p;
      PFETCH_S(c);
      if (esc != 0) {
        esc = 0;
        if (c == '\\' || c == cterm || c == ',') {
          /* */
        }
        else {
          e = eesc;
          cn++;
        }
        goto add_char;
      }
      else {
        if (c == '\\') {
          esc = 1;
          eesc = e;
        }
        else if (c == cterm || c == ',')
          break;
        else {
          size_t clen;

        add_char:
          if (skip_mode == 0) {
            clen = p - e;
            if (bufend + clen > buf + MAX_CALLOUT_ARG_BYTE_LENGTH)
              return ONIGERR_INVALID_CALLOUT_ARG; /* too long argument */

            xmemcpy(bufend, e, clen);
            bufend += clen;
          }
          cn++;
        }
      }
    }

    if (cn != 0) {
      if (skip_mode == 0) {
        if ((types[n] & ONIG_TYPE_LONG) != 0) {
          int fixed = 0;
          if (cn > 0) {
            long rl;
            r = parse_long(enc, buf, bufend, 1, LONG_MAX, &rl);
            if (r == ONIG_NORMAL) {
              vals[n].l = rl;
              fixed = 1;
              types[n] = ONIG_TYPE_LONG;
            }
          }

          if (fixed == 0) {
            types[n] = (types[n] & ~ONIG_TYPE_LONG);
            if (types[n] == ONIG_TYPE_VOID)
              return ONIGERR_INVALID_CALLOUT_ARG;
          }
        }

        switch (types[n]) {
        case ONIG_TYPE_LONG:
          break;

        case ONIG_TYPE_CHAR:
          if (cn != 1) return ONIGERR_INVALID_CALLOUT_ARG;
          vals[n].c = ONIGENC_MBC_TO_CODE(enc, buf, bufend);
          break;

        case ONIG_TYPE_STRING:
          {
            UChar* rs = onigenc_strdup(enc, buf, bufend);
            CHECK_NULL_RETURN_MEMERR(rs);
            vals[n].s.start = rs;
            vals[n].s.end   = rs + (e - s);
          }
          break;

        case ONIG_TYPE_TAG:
          if (eesc != 0 || ! is_allowed_callout_tag_name(enc, s, e))
            return ONIGERR_INVALID_CALLOUT_TAG_NAME;

          vals[n].s.start = s;
          vals[n].s.end   = e;
          break;

        case ONIG_TYPE_VOID:
        case ONIG_TYPE_POINTER:
          return ONIGERR_PARSER_BUG;
          break;
        }
      }

      n++;
    }

    if (c == cterm) break;
  }

  if (c != cterm) return ONIGERR_INVALID_CALLOUT_PATTERN;

  *src = p;
  return n;
}