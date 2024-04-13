absl::string_view HeaderString::getStringView() const {
  if (type() == Type::Reference) {
    return getStrView(buffer_);
  }
  ASSERT(type() == Type::Inline);
  return {getInVec(buffer_).data(), getInVec(buffer_).size()};
}