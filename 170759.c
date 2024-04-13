bool InstanceKlass::is_sealed() const {
  return _permitted_subclasses != NULL &&
         _permitted_subclasses != Universe::the_empty_short_array();
}