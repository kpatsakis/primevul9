  uint32_t readListBegin(TType& elemType, uint32_t& size) {
    T_VIRTUAL_CALL();
    return readListBegin_virt(elemType, size);
  }