struct HashMap *urlParseQuery(const char *buf, int len) {
  struct HashMap *hashmap = newHashMap(urlDestroyHashMapEntry, NULL);
  urlParseQueryString(hashmap, buf, len);
  return hashmap;
}