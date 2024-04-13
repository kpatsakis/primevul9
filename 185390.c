int DynamicMetadataMapIterator::luaPairsIterator(lua_State* state) {
  if (current_ == parent_.streamInfo().dynamicMetadata().filter_metadata().end()) {
    parent_.iterator_.reset();
    return 0;
  }

  lua_pushstring(state, current_->first.c_str());
  Filters::Common::Lua::MetadataMapHelper::createTable(state, current_->second.fields());

  current_++;
  return 2;
}