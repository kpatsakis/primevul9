  LowerCaseString(LowerCaseString&& rhs) noexcept : string_(std::move(rhs.string_)) {
    ASSERT(valid());
  }