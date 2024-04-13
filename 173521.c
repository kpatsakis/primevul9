scan_unsigned_octal_number(UChar** src, UChar* end, int maxlen,
			   OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (! PEND && maxlen-- != 0) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_DIGIT(enc, c) && c < '8') {
      val = ODIGITVAL(c);
      if ((INT_MAX_LIMIT - val) / 8UL < num)
        return -1;  /* overflow */

      num = (num << 3) + val;
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}