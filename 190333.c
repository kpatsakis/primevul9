  uint32_t readMapBegin(TType& keyType, TType& valType, uint32_t& size) {
    T_VIRTUAL_CALL();
    return readMapBegin_virt(keyType, valType, size);
  }