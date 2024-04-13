Method* InstanceKlass::find_instance_method(const Symbol* name,
                                            const Symbol* signature,
                                            PrivateLookupMode private_mode) const {
  return InstanceKlass::find_instance_method(methods(), name, signature, private_mode);
}