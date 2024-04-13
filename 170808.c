void InstanceKlass::clean_method_data() {
  for (int m = 0; m < methods()->length(); m++) {
    MethodData* mdo = methods()->at(m)->method_data();
    if (mdo != NULL) {
      MutexLocker ml(SafepointSynchronize::is_at_safepoint() ? NULL : mdo->extra_data_lock());
      mdo->clean_method_data(/*always_clean*/false);
    }
  }
}