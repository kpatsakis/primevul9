oop InstanceKlass::protection_domain() const {
  // return the protection_domain from the mirror
  return java_lang_Class::protection_domain(java_mirror());
}