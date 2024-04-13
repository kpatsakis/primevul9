void InstanceKlass::check_prohibited_package(Symbol* class_name,
                                             ClassLoaderData* loader_data,
                                             TRAPS) {
  if (!loader_data->is_boot_class_loader_data() &&
      !loader_data->is_platform_class_loader_data() &&
      class_name != NULL && class_name->utf8_length() >= 5) {
    ResourceMark rm(THREAD);
    bool prohibited;
    const jbyte* base = class_name->base();
    if ((base[0] | base[1] | base[2] | base[3] | base[4]) & 0x80) {
      prohibited = is_prohibited_package_slow(class_name);
    } else {
      char* name = class_name->as_C_string();
      prohibited = (strncmp(name, JAVAPKG, JAVAPKG_LEN) == 0 && name[JAVAPKG_LEN] == '/');
    }
    if (prohibited) {
      TempNewSymbol pkg_name = InstanceKlass::package_from_name(class_name, CHECK);
      assert(pkg_name != NULL, "Error in parsing package name starting with 'java/'");
      char* name = pkg_name->as_C_string();
      const char* class_loader_name = loader_data->loader_name_and_id();
      StringUtils::replace_no_expand(name, "/", ".");
      const char* msg_text1 = "Class loader (instance of): ";
      const char* msg_text2 = " tried to load prohibited package name: ";
      size_t len = strlen(msg_text1) + strlen(class_loader_name) + strlen(msg_text2) + strlen(name) + 1;
      char* message = NEW_RESOURCE_ARRAY_IN_THREAD(THREAD, char, len);
      jio_snprintf(message, len, "%s%s%s%s", msg_text1, class_loader_name, msg_text2, name);
      THROW_MSG(vmSymbols::java_lang_SecurityException(), message);
    }
  }
  return;
}