void HeaderMapImpl::updateSize(uint64_t from_size, uint64_t to_size) {
  ASSERT(cached_byte_size_ >= from_size);
  cached_byte_size_ -= from_size;
  cached_byte_size_ += to_size;
}