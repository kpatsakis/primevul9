const char* InstanceKlass::signature_name() const {
  int hash_len = 0;
  char hash_buf[40];

  // If this is an anonymous class, append a hash to make the name unique
  if (is_anonymous()) {
    intptr_t hash = (java_mirror() != NULL) ? java_mirror()->identity_hash() : 0;
    jio_snprintf(hash_buf, sizeof(hash_buf), "/" UINTX_FORMAT, (uintx)hash);
    hash_len = (int)strlen(hash_buf);
  }

  // Get the internal name as a c string
  const char* src = (const char*) (name()->as_C_string());
  const int src_length = (int)strlen(src);

  char* dest = NEW_RESOURCE_ARRAY(char, src_length + hash_len + 3);

  // Add L as type indicator
  int dest_index = 0;
  dest[dest_index++] = 'L';

  // Add the actual class name
  for (int src_index = 0; src_index < src_length; ) {
    dest[dest_index++] = src[src_index++];
  }

  // If we have a hash, append it
  for (int hash_index = 0; hash_index < hash_len; ) {
    dest[dest_index++] = hash_buf[hash_index++];
  }

  // Add the semicolon and the NULL
  dest[dest_index++] = ';';
  dest[dest_index] = '\0';
  return dest;
}