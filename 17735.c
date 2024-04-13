void InstanceKlass::ensure_space_for_methodids(int start_offset) {
  int new_jmeths = 0;
  int length = methods()->length();
  for (int index = start_offset; index < length; index++) {
    Method* m = methods()->at(index);
    jmethodID id = m->find_jmethod_id_or_null();
    if (id == NULL) {
      new_jmeths++;
    }
  }
  if (new_jmeths != 0) {
    Method::ensure_jmethod_ids(class_loader_data(), new_jmeths);
  }
}