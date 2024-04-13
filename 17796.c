void InstanceKlass::deallocate_methods(ClassLoaderData* loader_data,
                                       Array<Method*>* methods) {
  if (methods != NULL && methods != Universe::the_empty_method_array() &&
      !methods->is_shared()) {
    for (int i = 0; i < methods->length(); i++) {
      Method* method = methods->at(i);
      if (method == NULL) continue;  // maybe null if error processing
      // Only want to delete methods that are not executing for RedefineClasses.
      // The previous version will point to them so they're not totally dangling
      assert (!method->on_stack(), "shouldn't be called with methods on stack");
      MetadataFactory::free_metadata(loader_data, method);
    }
    MetadataFactory::free_array<Method*>(loader_data, methods);
  }
}