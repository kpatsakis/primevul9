void HeaderString::setInteger(uint64_t value) {
  // Initialize the size to the max length, copy the actual data, and then
  // reduce the size (but not the capacity) as needed
  // Note: instead of using the inner_buffer, attempted the following:
  // resize buffer_ to MaxIntegerLength, apply StringUtil::itoa to the buffer_.data(), and then
  // resize buffer_ to int_length (the number of digits in value).
  // However it was slower than the following approach.
  char inner_buffer[MaxIntegerLength];
  const uint32_t int_length = StringUtil::itoa(inner_buffer, MaxIntegerLength, value);

  if (type() == Type::Reference) {
    // Switching from Type::Reference to Type::Inline
    buffer_ = InlineHeaderVector();
  }
  ASSERT((getInVec(buffer_).capacity()) > MaxIntegerLength);
  getInVec(buffer_).assign(inner_buffer, inner_buffer + int_length);
}