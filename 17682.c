void InstanceKlass::mark_newly_obsolete_methods(Array<Method*>* old_methods,
                                                int emcp_method_count) {
  int obsolete_method_count = old_methods->length() - emcp_method_count;

  if (emcp_method_count != 0 && obsolete_method_count != 0 &&
      _previous_versions != NULL) {
    // We have a mix of obsolete and EMCP methods so we have to
    // clear out any matching EMCP method entries the hard way.
    int local_count = 0;
    for (int i = 0; i < old_methods->length(); i++) {
      Method* old_method = old_methods->at(i);
      if (old_method->is_obsolete()) {
        // only obsolete methods are interesting
        Symbol* m_name = old_method->name();
        Symbol* m_signature = old_method->signature();

        // previous versions are linked together through the InstanceKlass
        int j = 0;
        for (InstanceKlass* prev_version = _previous_versions;
             prev_version != NULL;
             prev_version = prev_version->previous_versions(), j++) {

          Array<Method*>* method_refs = prev_version->methods();
          for (int k = 0; k < method_refs->length(); k++) {
            Method* method = method_refs->at(k);

            if (!method->is_obsolete() &&
                method->name() == m_name &&
                method->signature() == m_signature) {
              // The current RedefineClasses() call has made all EMCP
              // versions of this method obsolete so mark it as obsolete
              log_trace(redefine, class, iklass, add)
                ("%s(%s): flush obsolete method @%d in version @%d",
                 m_name->as_C_string(), m_signature->as_C_string(), k, j);

              method->set_is_obsolete();
              break;
            }
          }

          // The previous loop may not find a matching EMCP method, but
          // that doesn't mean that we can optimize and not go any
          // further back in the PreviousVersion generations. The EMCP
          // method for this generation could have already been made obsolete,
          // but there still may be an older EMCP method that has not
          // been made obsolete.
        }

        if (++local_count >= obsolete_method_count) {
          // no more obsolete methods so bail out now
          break;
        }
      }
    }
  }
}