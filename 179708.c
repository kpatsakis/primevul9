static char *urlUnescape(char *s) {
  int warned    = 0;
  char *r       = s;
  for (char *u  = s; *u; ) {
    char ch     = *u++;
    if (ch == '+') {
      ch        = ' ';
    } else if (ch == '%') {
      char c1   = *u;
      if ((c1 >= '0' && c1 <= '9') || ((c1 &= ~0x20) >= 'A' && c1 <= 'F')) {
        ch      = c1 - (c1 > '9' ? 'A' - 10 : '0');
        char c2 = *++u;
        if ((c2 >= '0' && c2 <= '9') || ((c2 &= ~0x20) >= 'A' && c2 <= 'F')) {
          ch    = (ch << 4) + c2 - (c2 > '9' ? 'A' - 10 : '0');
          ++u;
        } else if (!warned++) {
          warn("[http] Malformed URL encoded data \"%s\"!", r);
        }
      } else if (!warned++) {
        warn("[http] Malformed URL encoded data \"%s\"!", r);
      }
    }
    *s++        = ch;
  }
  *s            = '\000';
  return r;
}