static inline bool is_class_loader(const Symbol* class_name,
                                   const ClassFileParser& parser) {
  assert(class_name != NULL, "invariant");

  if (class_name == vmSymbols::java_lang_ClassLoader()) {
    return true;
  }

  if (SystemDictionary::ClassLoader_klass_loaded()) {
    const Klass* const super_klass = parser.super_klass();
    if (super_klass != NULL) {
      if (super_klass->is_subtype_of(SystemDictionary::ClassLoader_klass())) {
        return true;
      }
    }
  }
  return false;
}