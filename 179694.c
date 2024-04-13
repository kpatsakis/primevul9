static void urlParseHeaderLine(struct HashMap *hashmap, const char *s,
                               int len) {
  while (s && len > 0) {
    while (len > 0 && (*s == ' ' || *s == ';')) {
      s++;
      len--;
    }
    const char *key   = s;
    const char *value = NULL;
    while (len > 0 && *s != ';') {
      if (*s == '=' && value == NULL) {
        value         = s + 1;
      }
      s++;
      len--;
    }
    int kl            = (value ? value-1 : s) - key;
    int vl            = value ? s - value : 0;
    if (kl) {
      char *k         = urlMakeString(key, kl);
      for (char *t = k; *t; t++) {
        if (*t >= 'a' && *t <= 'z') {
          *t         |= 0x20;
        }
      }
      char *v         = NULL;
      if (value) {
        if (vl >= 2 && value[0] == '"' && value[vl-1] == '"') {
          value++;
          vl--;
        }
        v             = urlMakeString(value, vl);
      }
      addToHashMap(hashmap, k, v);
    }
  }
}