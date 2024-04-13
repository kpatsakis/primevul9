uint32_t TProtocol::skip_virt(TType type) {
  return ::apache::thrift::protocol::skip(*this, type);
}