int HeaderMapIterator::luaPairsIterator(lua_State* state) {
  if (current_ == entries_.size()) {
    parent_.iterator_.reset();
    return 0;
  } else {
    const absl::string_view key_view(entries_[current_]->key().getStringView());
    lua_pushlstring(state, key_view.data(), key_view.length());
    const absl::string_view value_view(entries_[current_]->value().getStringView());
    lua_pushlstring(state, value_view.data(), value_view.length());
    current_++;
    return 2;
  }
}