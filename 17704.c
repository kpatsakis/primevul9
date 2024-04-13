void InstanceKlass::get_jmethod_id_length_value(jmethodID* cache,
       size_t idnum, size_t *length_p, jmethodID* id_p) {
  assert(cache != NULL, "sanity check");
  assert(length_p != NULL, "sanity check");
  assert(id_p != NULL, "sanity check");

  // cache size is stored in element[0], other elements offset by one
  *length_p = (size_t)cache[0];
  if (*length_p <= idnum) {  // cache is too short
    *id_p = NULL;
  } else {
    *id_p = cache[idnum+1];  // fetch jmethodID (if any)
  }
}