  std::string get_(const LowerCaseString& key) const {
    const HeaderEntry* header = get(key);
    if (!header) {
      return EMPTY_STRING;
    } else {
      return std::string(header->value().getStringView());
    }
  }