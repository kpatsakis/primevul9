void InstanceKlass::print_class_load_logging(ClassLoaderData* loader_data,
                                             const char* module_name,
                                             const ClassFileStream* cfs) const {
  if (!log_is_enabled(Info, class, load)) {
    return;
  }

  ResourceMark rm;
  LogMessage(class, load) msg;
  stringStream info_stream;

  // Name and class hierarchy info
  info_stream.print("%s", external_name());

  // Source
  if (cfs != NULL) {
    if (cfs->source() != NULL) {
      if (module_name != NULL) {
        if (ClassLoader::is_modules_image(cfs->source())) {
          info_stream.print(" source: jrt:/%s", module_name);
        } else {
          info_stream.print(" source: %s", cfs->source());
        }
      } else {
        info_stream.print(" source: %s", cfs->source());
      }
    } else if (loader_data == ClassLoaderData::the_null_class_loader_data()) {
      Thread* THREAD = Thread::current();
      Klass* caller =
            THREAD->is_Java_thread()
                ? ((JavaThread*)THREAD)->security_get_caller_class(1)
                : NULL;
      // caller can be NULL, for example, during a JVMTI VM_Init hook
      if (caller != NULL) {
        info_stream.print(" source: instance of %s", caller->external_name());
      } else {
        // source is unknown
      }
    } else {
      oop class_loader = loader_data->class_loader();
      info_stream.print(" source: %s", class_loader->klass()->external_name());
    }
  } else {
    info_stream.print(" source: shared objects file");
  }

  msg.info("%s", info_stream.as_string());

  if (log_is_enabled(Debug, class, load)) {
    stringStream debug_stream;

    // Class hierarchy info
    debug_stream.print(" klass: " INTPTR_FORMAT " super: " INTPTR_FORMAT,
                       p2i(this),  p2i(superklass()));

    // Interfaces
    if (local_interfaces() != NULL && local_interfaces()->length() > 0) {
      debug_stream.print(" interfaces:");
      int length = local_interfaces()->length();
      for (int i = 0; i < length; i++) {
        debug_stream.print(" " INTPTR_FORMAT,
                           p2i(InstanceKlass::cast(local_interfaces()->at(i))));
      }
    }

    // Class loader
    debug_stream.print(" loader: [");
    loader_data->print_value_on(&debug_stream);
    debug_stream.print("]");

    // Classfile checksum
    if (cfs) {
      debug_stream.print(" bytes: %d checksum: %08x",
                         cfs->length(),
                         ClassLoader::crc32(0, (const char*)cfs->buffer(),
                         cfs->length()));
    }

    msg.debug("%s", debug_stream.as_string());
  }
}