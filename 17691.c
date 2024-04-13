void InstanceKlass::clean_weak_instanceklass_links() {
  clean_implementors_list();
  clean_method_data();

  // Since GC iterates InstanceKlasses sequentially, it is safe to remove stale entries here.
  DependencyContext dep_context(&_dep_context);
  dep_context.expunge_stale_entries();
}