onig_snprintf_with_pattern(buf, bufsize, enc, pat, pat_end, fmt, va_alist)
    UChar buf[];
    int bufsize;
    OnigEncoding enc;
    UChar* pat;
    UChar* pat_end;
    const UChar *fmt;
    va_dcl
#endif
{
  int n, need, len;
  UChar *p, *s, *bp;
  UChar bs[6];
  va_list args;

  va_init_list(args, fmt);
  n = xvsnprintf((char* )buf, bufsize, (const char* )fmt, args);
  va_end(args);

  need = (pat_end - pat) * 4 + 4;

  if (n + need < bufsize) {
    xstrcat((char* )buf, ": /", bufsize);
    s = buf + onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, buf);

    p = pat;
    while (p < pat_end) {
      if (*p == '\\') {
	*s++ = *p++;
	len = enclen(enc, p);
	while (len-- > 0) *s++ = *p++;
      }
      else if (*p == '/') {
	*s++ = (unsigned char )'\\';
	*s++ = *p++;
      }
      else if (ONIGENC_IS_MBC_HEAD(enc, p)) {
        len = enclen(enc, p);
        if (ONIGENC_MBC_MINLEN(enc) == 1) {
          while (len-- > 0) *s++ = *p++;
        }
        else { /* for UTF16 */
          int blen;

          while (len-- > 0) {
	    sprint_byte_with_x((char* )bs, (unsigned int )(*p++));
            blen = onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, bs);
            bp = bs;
            while (blen-- > 0) *s++ = *bp++;
          }
        }
      }
      else if (!ONIGENC_IS_CODE_PRINT(enc, *p) &&
	       !ONIGENC_IS_CODE_SPACE(enc, *p)) {
	sprint_byte_with_x((char* )bs, (unsigned int )(*p++));
	len = onigenc_str_bytelen_null(ONIG_ENCODING_ASCII, bs);
        bp = bs;
	while (len-- > 0) *s++ = *bp++;
      }
      else {
	*s++ = *p++;
      }
    }

    *s++ = '/';
    *s   = '\0';
  }
}