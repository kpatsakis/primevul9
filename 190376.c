  uint32_t readMessageBegin(std::string& name, TMessageType& messageType, int32_t& seqid) {
    T_VIRTUAL_CALL();
    return readMessageBegin_virt(name, messageType, seqid);
  }