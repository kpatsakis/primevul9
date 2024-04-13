jint InstanceKlass::jvmti_class_status() const {
  jint result = 0;

  if (is_linked()) {
    result |= JVMTI_CLASS_STATUS_VERIFIED | JVMTI_CLASS_STATUS_PREPARED;
  }

  if (is_initialized()) {
    assert(is_linked(), "Class status is not consistent");
    result |= JVMTI_CLASS_STATUS_INITIALIZED;
  }
  if (is_in_error_state()) {
    result |= JVMTI_CLASS_STATUS_ERROR;
  }
  return result;
}