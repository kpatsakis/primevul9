bool InstanceKlass::is_record() const {
  return _record_components != NULL &&
         is_final() &&
         java_super() == vmClasses::Record_klass();
}