bool InstanceKlass::is_same_class_package(const Klass* class2) const {
  oop classloader1 = this->class_loader();
  PackageEntry* classpkg1 = this->package();
  if (class2->is_objArray_klass()) {
    class2 = ObjArrayKlass::cast(class2)->bottom_klass();
  }

  oop classloader2;
  PackageEntry* classpkg2;
  if (class2->is_instance_klass()) {
    classloader2 = class2->class_loader();
    classpkg2 = class2->package();
  } else {
    assert(class2->is_typeArray_klass(), "should be type array");
    classloader2 = NULL;
    classpkg2 = NULL;
  }

  // Same package is determined by comparing class loader
  // and package entries. Both must be the same. This rule
  // applies even to classes that are defined in the unnamed
  // package, they still must have the same class loader.
  if ((classloader1 == classloader2) && (classpkg1 == classpkg2)) {
    return true;
  }

  return false;
}