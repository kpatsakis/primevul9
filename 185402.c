uint32_t HeaderString::size() const {
  if (type() == Type::Reference) {
    return getStrView(buffer_).size();
  }
  ASSERT(type() == Type::Inline);
  return getInVec(buffer_).size();
}