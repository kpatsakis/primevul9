static char *urlMakeString(const char *buf, int len) {
  if (!buf) {
    return NULL;
  } else {
    char *s;
    check(s = malloc(len + 1));
    memcpy(s, buf, len);
    s[len]  = '\000';
    return s;
  }
}