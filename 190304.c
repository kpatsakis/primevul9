  uint32_t writeMapBegin(const TType keyType, const TType valType, const uint32_t size) {
    T_VIRTUAL_CALL();
    return writeMapBegin_virt(keyType, valType, size);
  }