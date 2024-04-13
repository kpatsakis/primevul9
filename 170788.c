void InstanceKlass::log_to_classlist() const {
#if INCLUDE_CDS
  ResourceMark rm;
  if (ClassListWriter::is_enabled()) {
    if (!ClassLoader::has_jrt_entry()) {
       warning("DumpLoadedClassList and CDS are not supported in exploded build");
       DumpLoadedClassList = NULL;
       return;
    }
    if (is_shareable()) {
      ClassListWriter w;
      w.stream()->print_cr("%s", name()->as_C_string());
      w.stream()->flush();
    }
  }
#endif // INCLUDE_CDS
}