onig_scan_unsigned_number(UChar** src, const UChar* end, OnigEncoding enc)
{
  unsigned int num, val;
  OnigCodePoint c;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (! PEND) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_DIGIT(enc, c)) {
      val = (unsigned int )DIGITVAL(c);
      if ((INT_MAX_LIMIT - val) / 10UL < num)
        return -1;  /* overflow */

      num = num * 10 + val;
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}