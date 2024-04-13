bool InstanceKlass::find_local_field(Symbol* name, Symbol* sig, fieldDescriptor* fd) const {
  for (JavaFieldStream fs(this); !fs.done(); fs.next()) {
    Symbol* f_name = fs.name();
    Symbol* f_sig  = fs.signature();
    if (f_name == name && f_sig == sig) {
      fd->reinitialize(const_cast<InstanceKlass*>(this), fs.index());
      return true;
    }
  }
  return false;
}