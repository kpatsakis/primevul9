InstanceKlass* InstanceKlass::nest_host(Symbol* validationException, TRAPS) {
  InstanceKlass* nest_host_k = _nest_host;
  if (nest_host_k == NULL) {
    // need to resolve and save our nest-host class. This could be attempted
    // concurrently but as the result is idempotent and we don't use the class
    // then we do not need any synchronization beyond what is implicitly used
    // during class loading.
    if (_nest_host_index != 0) { // we have a real nest_host
      // Before trying to resolve check if we're in a suitable context
      if (!THREAD->can_call_java() && !_constants->tag_at(_nest_host_index).is_klass()) {
        if (log_is_enabled(Trace, class, nestmates)) {
          ResourceMark rm(THREAD);
          log_trace(class, nestmates)("Rejected resolution of nest-host of %s in unsuitable thread",
                                      this->external_name());
        }
        return NULL;
      }

      if (log_is_enabled(Trace, class, nestmates)) {
        ResourceMark rm(THREAD);
        log_trace(class, nestmates)("Resolving nest-host of %s using cp entry for %s",
                                    this->external_name(),
                                    _constants->klass_name_at(_nest_host_index)->as_C_string());
      }

      Klass* k = _constants->klass_at(_nest_host_index, THREAD);
      if (HAS_PENDING_EXCEPTION) {
        Handle exc_h = Handle(THREAD, PENDING_EXCEPTION);
        if (exc_h->is_a(SystemDictionary::NoClassDefFoundError_klass())) {
          // throw a new CDNFE with the original as its cause, and a clear msg
          ResourceMark rm(THREAD);
          char buf[200];
          CLEAR_PENDING_EXCEPTION;
          jio_snprintf(buf, sizeof(buf),
                       "Unable to load nest-host class (%s) of %s",
                       _constants->klass_name_at(_nest_host_index)->as_C_string(),
                       this->external_name());
          log_trace(class, nestmates)("%s - NoClassDefFoundError", buf);
          THROW_MSG_CAUSE_NULL(vmSymbols::java_lang_NoClassDefFoundError(), buf, exc_h);
        }
        // All other exceptions pass through (OOME, StackOverflowError, LinkageErrors etc).
        return NULL;
      }

      // A valid nest-host is an instance class in the current package that lists this
      // class as a nest member. If any of these conditions are not met we post the
      // requested exception type (if any) and return NULL

      const char* error = NULL;

      // JVMS 5.4.4 indicates package check comes first
      if (is_same_class_package(k)) {

        // Now check actual membership. We can't be a member if our "host" is
        // not an instance class.
        if (k->is_instance_klass()) {
          nest_host_k = InstanceKlass::cast(k);

          bool is_member = nest_host_k->has_nest_member(this, CHECK_NULL);
          if (is_member) {
            // save resolved nest-host value
            _nest_host = nest_host_k;

            if (log_is_enabled(Trace, class, nestmates)) {
              ResourceMark rm(THREAD);
              log_trace(class, nestmates)("Resolved nest-host of %s to %s",
                                          this->external_name(), k->external_name());
            }
            return nest_host_k;
          }
        }
        error = "current type is not listed as a nest member";
      } else {
        error = "types are in different packages";
      }

      if (log_is_enabled(Trace, class, nestmates)) {
        ResourceMark rm(THREAD);
        log_trace(class, nestmates)("Type %s is not a nest member of resolved type %s: %s",
                                    this->external_name(),
                                    k->external_name(),
                                    error);
      }

      if (validationException != NULL && THREAD->can_call_java()) {
        ResourceMark rm(THREAD);
        Exceptions::fthrow(THREAD_AND_LOCATION,
                           validationException,
                           "Type %s is not a nest member of %s: %s",
                           this->external_name(),
                           k->external_name(),
                           error
                           );
      }
      return NULL;
    } else {
      if (log_is_enabled(Trace, class, nestmates)) {
        ResourceMark rm(THREAD);
        log_trace(class, nestmates)("Type %s is not part of a nest: setting nest-host to self",
                                    this->external_name());
      }
      // save resolved nest-host value
      return (_nest_host = this);
    }
  }
  return nest_host_k;
}