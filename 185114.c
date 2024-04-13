int HeaderMapWrapper::luaPairs(lua_State* state) {
  if (iterator_.get() != nullptr) {
    luaL_error(state, "cannot create a second iterator before completing the first");
  }

  // The way iteration works is we create an iteration wrapper that snaps pointers to all of
  // the headers. We don't allow modification while an iterator is active. This means that
  // currently if a script breaks out of iteration, further modifications will not be possible
  // because we don't know if they may resume iteration in the future and it isn't safe. There
  // are potentially better ways of handling this but due to GC of the iterator it's very
  // difficult to control safety without tracking every allocated iterator and invalidating them
  // if the map is modified.
  iterator_.reset(HeaderMapIterator::create(state, *this), true);
  lua_pushcclosure(state, HeaderMapIterator::static_luaPairsIterator, 1);
  return 1;
}