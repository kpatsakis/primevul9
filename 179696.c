static int urlMemcasecmp(const char *buf, int len, const char *s) {
  int sLen = strlen(s);
  if (len < sLen) {
    return s[len];
  } else {
    return strncasecmp(buf, s, sLen);
  }
}