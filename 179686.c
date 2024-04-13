static int urlMemcmp(const char *buf, int len, const char *s) {
  int sLen = strlen(s);
  if (len < sLen) {
    return s[len];
  } else {
    return memcmp(buf, s, sLen);
  }
}