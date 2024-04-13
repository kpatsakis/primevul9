  template <class It> static void initFromInitList(HeaderMap& new_header_map, It begin, It end) {
    for (auto it = begin; it != end; ++it) {
      static_assert(std::is_same<decltype(it->first), LowerCaseString>::value,
                    "iterator must be pair and the first value of them must be LowerCaseString");
      HeaderString key_string;
      key_string.setCopy(it->first.get().c_str(), it->first.get().size());
      HeaderString value_string;
      value_string.setCopy(it->second.c_str(), it->second.size());
      new_header_map.addViaMove(std::move(key_string), std::move(value_string));
    }
  }