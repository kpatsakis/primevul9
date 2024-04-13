static void urlDestroyHashMapEntry(void *arg ATTR_UNUSED, char *key,
                                   char *value) {
  UNUSED(arg);
  free(key);
  free(value);
}