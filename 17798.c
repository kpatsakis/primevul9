bool InstanceKlass::is_override(const methodHandle& super_method, Handle targetclassloader, Symbol* targetclassname, TRAPS) {
   // Private methods can not be overridden
   if (super_method->is_private()) {
     return false;
   }
   // If super method is accessible, then override
   if ((super_method->is_protected()) ||
       (super_method->is_public())) {
     return true;
   }
   // Package-private methods are not inherited outside of package
   assert(super_method->is_package_private(), "must be package private");
   return(is_same_class_package(targetclassloader(), targetclassname));
}