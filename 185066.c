void HeaderMapImpl::subtractSize(uint64_t size) {
  ASSERT(cached_byte_size_ >= size);
  cached_byte_size_ -= size;
}