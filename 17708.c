bool InstanceKlass::is_same_class_package(oop other_class_loader,
                                          const Symbol* other_class_name) const {
  if (class_loader() != other_class_loader) {
    return false;
  }
  if (name()->fast_compare(other_class_name) == 0) {
     return true;
  }

  {
    ResourceMark rm;

    bool bad_class_name = false;
    const char* other_pkg =
      ClassLoader::package_from_name((const char*) other_class_name->as_C_string(), &bad_class_name);
    if (bad_class_name) {
      return false;
    }
    // Check that package_from_name() returns NULL, not "", if there is no package.
    assert(other_pkg == NULL || strlen(other_pkg) > 0, "package name is empty string");

    const Symbol* const this_package_name =
      this->package() != NULL ? this->package()->name() : NULL;

    if (this_package_name == NULL || other_pkg == NULL) {
      // One of the two doesn't have a package.  Only return true if the other
      // one also doesn't have a package.
      return (const char*)this_package_name == other_pkg;
    }

    // Check if package is identical
    return this_package_name->equals(other_pkg);
  }
}