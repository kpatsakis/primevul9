bool InstanceKlass::should_store_fingerprint(bool is_anonymous) {
#if INCLUDE_AOT
  // We store the fingerprint into the InstanceKlass only in the following 2 cases:
  if (CalculateClassFingerprint) {
    // (1) We are running AOT to generate a shared library.
    return true;
  }
  if (DumpSharedSpaces) {
    // (2) We are running -Xshare:dump to create a shared archive
    return true;
  }
  if (UseAOT && is_anonymous) {
    // (3) We are using AOT code from a shared library and see an anonymous class
    return true;
  }
#endif

  // In all other cases we might set the _misc_has_passed_fingerprint_check bit,
  // but do not store the 64-bit fingerprint to save space.
  return false;
}