Method* InstanceKlass::find_instance_method(const Array<Method*>* methods,
                                            const Symbol* name,
                                            const Symbol* signature,
                                            PrivateLookupMode private_mode) {
  Method* const meth = InstanceKlass::find_method_impl(methods,
                                                 name,
                                                 signature,
                                                 find_overpass,
                                                 skip_static,
                                                 private_mode);
  assert(((meth == NULL) || !meth->is_static()),
    "find_instance_method should have skipped statics");
  return meth;
}