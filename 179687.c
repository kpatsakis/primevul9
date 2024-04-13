static void urlParseQueryString(struct HashMap *hashmap, const char *query, int len) {
  const char *key   = query;
  const char *value = NULL;
  for (const char *ampersand = query; len-- >= 0; ampersand++) {
    char ch         = len >= 0 ? *ampersand : '\000';
    if (ch == '=' && !value) {
      value         = ampersand + 1;
    } else if (ch == '&' || len < 0) {
      int kl        = (value ? value-1 : ampersand) - key;
      int vl        = value ? ampersand - value : 0;
      if (kl) {
        char *k     = urlMakeString(key, kl);
        urlUnescape(k);
        char *v     = NULL;
        if (value) {
          v         = urlMakeString(value, vl);
          urlUnescape(v);
        }
        addToHashMap(hashmap, k, v);
      }
      key           = ampersand + 1;
      value         = NULL;
    }
    if (!ch) {
      break;
    }
  }
}