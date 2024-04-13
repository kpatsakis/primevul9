void InstanceKlass::oop_verify_on(oop obj, outputStream* st) {
  Klass::oop_verify_on(obj, st);
  VerifyFieldClosure blk;
  obj->oop_iterate(&blk);
}