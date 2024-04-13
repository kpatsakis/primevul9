static const char *urlMemstr(const char *buf, int len, const char *s) {
  int sLen        = strlen(s);
  if (!sLen) {
    return buf;
  }
  while (len >= sLen) {
    if (len > sLen) {
      char *first = memchr(buf, *s, len - sLen);
      if (!first) {
        return NULL;
      }
      len        -= first - buf;
      buf         = first;
    }
    if (!memcmp(buf, s, sLen)) {
      return buf;
    }
    buf++;
    len--;
  }
  return NULL;
}