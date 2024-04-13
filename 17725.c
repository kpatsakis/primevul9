void InstanceKlass::deallocate_interfaces(ClassLoaderData* loader_data,
                                          const Klass* super_klass,
                                          Array<Klass*>* local_interfaces,
                                          Array<Klass*>* transitive_interfaces) {
  // Only deallocate transitive interfaces if not empty, same as super class
  // or same as local interfaces.  See code in parseClassFile.
  Array<Klass*>* ti = transitive_interfaces;
  if (ti != Universe::the_empty_klass_array() && ti != local_interfaces) {
    // check that the interfaces don't come from super class
    Array<Klass*>* sti = (super_klass == NULL) ? NULL :
                    InstanceKlass::cast(super_klass)->transitive_interfaces();
    if (ti != sti && ti != NULL && !ti->is_shared()) {
      MetadataFactory::free_array<Klass*>(loader_data, ti);
    }
  }

  // local interfaces can be empty
  if (local_interfaces != Universe::the_empty_klass_array() &&
      local_interfaces != NULL && !local_interfaces->is_shared()) {
    MetadataFactory::free_array<Klass*>(loader_data, local_interfaces);
  }
}