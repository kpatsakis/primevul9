klassItable InstanceKlass::itable() const {
  return klassItable(const_cast<InstanceKlass*>(this));
}