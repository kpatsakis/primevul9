Method* InstanceKlass::find_method(const Array<Method*>* methods,
                                   const Symbol* name,
                                   const Symbol* signature) {
  return InstanceKlass::find_method_impl(methods,
                                         name,
                                         signature,
                                         find_overpass,
                                         find_static,
                                         find_private);
}