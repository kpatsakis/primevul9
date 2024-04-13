void InstanceKlass::assign_class_loader_type() {
  ClassLoaderData *cld = class_loader_data();
  if (cld->is_boot_class_loader_data()) {
    set_shared_class_loader_type(ClassLoader::BOOT_LOADER);
  }
  else if (cld->is_platform_class_loader_data()) {
    set_shared_class_loader_type(ClassLoader::PLATFORM_LOADER);
  }
  else if (cld->is_system_class_loader_data()) {
    set_shared_class_loader_type(ClassLoader::APP_LOADER);
  }
}