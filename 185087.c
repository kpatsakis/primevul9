  static Handle<type> registerInlineHeader(const LowerCaseString& header_name) {
    static size_t inline_header_index = 0;

    ASSERT(!mutableFinalized<type>());
    auto& map = mutableRegistrationMap<type>();
    auto entry = map.find(header_name);
    if (entry == map.end()) {
      map[header_name] = inline_header_index++;
    }
    return Handle<type>(map.find(header_name));
  }