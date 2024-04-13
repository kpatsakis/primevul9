Method* InstanceKlass::find_method(const Symbol* name,
                                   const Symbol* signature) const {
  return find_method_impl(name, signature, find_overpass, find_static, find_private);
}