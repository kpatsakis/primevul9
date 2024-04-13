  uint32_t writeMessageBegin(const std::string& name,
                             const TMessageType messageType,
                             const int32_t seqid) {
    T_VIRTUAL_CALL();
    return writeMessageBegin_virt(name, messageType, seqid);
  }