HeaderString::HeaderString() : buffer_(InlineHeaderVector()) {
  ASSERT((getInVec(buffer_).capacity()) >= MaxIntegerLength);
  ASSERT(valid());
}