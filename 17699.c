Array<int>* InstanceKlass::create_new_default_vtable_indices(int len, TRAPS) {
  Array<int>* vtable_indices = MetadataFactory::new_array<int>(class_loader_data(), len, CHECK_NULL);
  assert(default_vtable_indices() == NULL, "only create once");
  set_default_vtable_indices(vtable_indices);
  return vtable_indices;
}