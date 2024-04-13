bool InstanceKlass::verify_code(TRAPS) {
  // 1) Verify the bytecodes
  return Verifier::verify(this, should_verify_class(), THREAD);
}