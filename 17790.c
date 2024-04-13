void InstanceKlass::notify_unload_class(InstanceKlass* ik) {
  // notify the debugger
  if (JvmtiExport::should_post_class_unload()) {
    JvmtiExport::post_class_unload(ik);
  }

  // notify ClassLoadingService of class unload
  ClassLoadingService::notify_class_unloaded(ik);
}