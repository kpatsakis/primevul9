void InstanceKlass::set_shared_class_loader_type(s2 loader_type) {
  switch (loader_type) {
  case ClassLoader::BOOT_LOADER:
    _misc_flags |= _misc_is_shared_boot_class;
    break;
  case ClassLoader::PLATFORM_LOADER:
    _misc_flags |= _misc_is_shared_platform_class;
    break;
  case ClassLoader::APP_LOADER:
    _misc_flags |= _misc_is_shared_app_class;
    break;
  default:
    ShouldNotReachHere();
    break;
  }
}