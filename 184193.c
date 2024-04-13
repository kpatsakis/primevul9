onig_is_code_in_cc(OnigEncoding enc, OnigCodePoint code, CClassNode* cc)
{
  int len;

  if (ONIGENC_MBC_MINLEN(enc) > 1) {
    len = 2;
  }
  else {
    len = ONIGENC_CODE_TO_MBCLEN(enc, code);
    if (len < 0) return 0;
  }
  return onig_is_code_in_cc_len(len, code, cc);
}