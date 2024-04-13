void InstanceKlass::copy_method_ordering(const intArray* m, TRAPS) {
  if (m != NULL) {
    // allocate a new array and copy contents (memcpy?)
    _method_ordering = MetadataFactory::new_array<int>(class_loader_data(), m->length(), CHECK);
    for (int i = 0; i < m->length(); i++) {
      _method_ordering->at_put(i, m->at(i));
    }
  } else {
    _method_ordering = Universe::the_empty_int_array();
  }
}