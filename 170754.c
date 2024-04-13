bool InstanceKlass::is_shareable() const {
#if INCLUDE_CDS
  ClassLoaderData* loader_data = class_loader_data();
  if (!SystemDictionaryShared::is_sharing_possible(loader_data)) {
    return false;
  }

  if (is_hidden()) {
    return false;
  }

  if (module()->is_patched()) {
    return false;
  }

  return true;
#else
  return false;
#endif
}