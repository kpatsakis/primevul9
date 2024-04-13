int DynamicMetadataMapWrapper::luaPairs(lua_State* state) {
  if (iterator_.get() != nullptr) {
    luaL_error(state, "cannot create a second iterator before completing the first");
  }

  iterator_.reset(DynamicMetadataMapIterator::create(state, *this), true);
  lua_pushcclosure(state, DynamicMetadataMapIterator::static_luaPairsIterator, 1);
  return 1;
}