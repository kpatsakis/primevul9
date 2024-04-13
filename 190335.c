  uint32_t readFieldBegin(std::string& name, TType& fieldType, int16_t& fieldId) {
    T_VIRTUAL_CALL();
    return readFieldBegin_virt(name, fieldType, fieldId);
  }