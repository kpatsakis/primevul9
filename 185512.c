void HeaderString::rtrim() {
  ASSERT(type() == Type::Inline);
  absl::string_view original = getStringView();
  absl::string_view rtrimmed = StringUtil::rtrim(original);
  if (original.size() != rtrimmed.size()) {
    getInVec(buffer_).resize(rtrimmed.size());
  }
}