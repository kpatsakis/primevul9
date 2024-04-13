const char* InstanceKlass::signature_name() const {
  int hash_len = 0;
  char hash_buf[40];

  // Get the internal name as a c string
  const char* src = (const char*) (name()->as_C_string());
  const int src_length = (int)strlen(src);

  char* dest = NEW_RESOURCE_ARRAY(char, src_length + hash_len + 3);

  // Add L as type indicator
  int dest_index = 0;
  dest[dest_index++] = JVM_SIGNATURE_CLASS;

  // Add the actual class name
  for (int src_index = 0; src_index < src_length; ) {
    dest[dest_index++] = src[src_index++];
  }

  if (is_hidden()) { // Replace the last '+' with a '.'.
    for (int index = (int)src_length; index > 0; index--) {
      if (dest[index] == '+') {
        dest[index] = JVM_SIGNATURE_DOT;
        break;
      }
    }
  }

  // If we have a hash, append it
  for (int hash_index = 0; hash_index < hash_len; ) {
    dest[dest_index++] = hash_buf[hash_index++];
  }

  // Add the semicolon and the NULL
  dest[dest_index++] = JVM_SIGNATURE_ENDCLASS;
  dest[dest_index] = '\0';
  return dest;
}