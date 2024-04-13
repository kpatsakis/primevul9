objArrayOop InstanceKlass::signers() const {
  // return the signers from the mirror
  return java_lang_Class::signers(java_mirror());
}