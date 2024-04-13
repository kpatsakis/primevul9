JNIid* InstanceKlass::jni_id_for_impl(int offset) {
  MutexLocker ml(JfieldIdCreation_lock);
  // Retry lookup after we got the lock
  JNIid* probe = jni_ids() == NULL ? NULL : jni_ids()->find(offset);
  if (probe == NULL) {
    // Slow case, allocate new static field identifier
    probe = new JNIid(this, offset, jni_ids());
    set_jni_ids(probe);
  }
  return probe;
}