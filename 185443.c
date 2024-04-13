  explicit LowerCaseString(const std::string& new_string) : string_(new_string) {
    ASSERT(valid());
    lower();
  }