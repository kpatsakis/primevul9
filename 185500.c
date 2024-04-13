void HeaderString::clear() {
  if (type() == Type::Inline) {
    getInVec(buffer_).clear();
  }
}