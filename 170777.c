void InstanceKlass::unload_class(InstanceKlass* ik) {
  // Release dependencies.
  ik->dependencies().remove_all_dependents();

  // notify the debugger
  if (JvmtiExport::should_post_class_unload()) {
    JvmtiExport::post_class_unload(ik);
  }

  // notify ClassLoadingService of class unload
  ClassLoadingService::notify_class_unloaded(ik);

  if (Arguments::is_dumping_archive()) {
    SystemDictionaryShared::remove_dumptime_info(ik);
  }

  if (log_is_enabled(Info, class, unload)) {
    ResourceMark rm;
    log_info(class, unload)("unloading class %s " INTPTR_FORMAT, ik->external_name(), p2i(ik));
  }

  Events::log_class_unloading(Thread::current(), ik);

#if INCLUDE_JFR
  assert(ik != NULL, "invariant");
  EventClassUnload event;
  event.set_unloadedClass(ik);
  event.set_definingClassLoader(ik->class_loader_data());
  event.commit();
#endif
}