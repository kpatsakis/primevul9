void InstanceKlass::set_source_debug_extension(const char* array, int length) {
  if (array == NULL) {
    _source_debug_extension = NULL;
  } else {
    // Adding one to the attribute length in order to store a null terminator
    // character could cause an overflow because the attribute length is
    // already coded with an u4 in the classfile, but in practice, it's
    // unlikely to happen.
    assert((length+1) > length, "Overflow checking");
    char* sde = NEW_C_HEAP_ARRAY(char, (length + 1), mtClass);
    for (int i = 0; i < length; i++) {
      sde[i] = array[i];
    }
    sde[length] = '\0';
    _source_debug_extension = sde;
  }
}