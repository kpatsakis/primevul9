void InstanceKlass::clean_weak_instanceklass_links() {
  clean_implementors_list();
  clean_method_data();
}