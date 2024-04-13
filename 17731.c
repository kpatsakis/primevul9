void InstanceKlass::store_fingerprint(uint64_t fingerprint) {
  address adr = adr_fingerprint();
  if (adr != NULL) {
    Bytes::put_native_u8(adr, (u8)fingerprint); // adr may not be 64-bit aligned

    ResourceMark rm;
    log_trace(class, fingerprint)("stored as " PTR64_FORMAT " for class %s", fingerprint, external_name());
  }
}