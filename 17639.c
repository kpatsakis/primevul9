Symbol* InstanceKlass::package_from_name(const Symbol* name, TRAPS) {
  if (name == NULL) {
    return NULL;
  } else {
    if (name->utf8_length() <= 0) {
      return NULL;
    }
    ResourceMark rm;
    const char* package_name = ClassLoader::package_from_name((const char*) name->as_C_string());
    if (package_name == NULL) {
      return NULL;
    }
    Symbol* pkg_name = SymbolTable::new_symbol(package_name, THREAD);
    return pkg_name;
  }
}