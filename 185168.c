void HeaderMapImpl::clear() {
  clearInline();
  headers_.clear();
  cached_byte_size_ = 0;
}