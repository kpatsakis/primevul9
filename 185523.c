void HeaderString::append(const char* data, uint32_t data_size) {
  // Make sure the requested memory allocation is below uint32_t::max
  const uint64_t new_capacity = static_cast<uint64_t>(data_size) + size();
  validateCapacity(new_capacity);
  ASSERT(validHeaderString(absl::string_view(data, data_size)));

  switch (type()) {
  case Type::Reference: {
    // Rather than be too clever and optimize this uncommon case, we switch to
    // Inline mode and copy.
    const absl::string_view prev = getStrView(buffer_);
    buffer_ = InlineHeaderVector();
    // Assigning new_capacity to avoid resizing when appending the new data
    getInVec(buffer_).reserve(new_capacity);
    getInVec(buffer_).assign(prev.begin(), prev.end());
    break;
  }
  case Type::Inline: {
    getInVec(buffer_).reserve(new_capacity);
    break;
  }
  }
  getInVec(buffer_).insert(getInVec(buffer_).end(), data, data + data_size);
}