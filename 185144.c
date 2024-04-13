int StreamInfoWrapper::luaDynamicMetadata(lua_State* state) {
  if (dynamic_metadata_wrapper_.get() != nullptr) {
    dynamic_metadata_wrapper_.pushStack();
  } else {
    dynamic_metadata_wrapper_.reset(DynamicMetadataMapWrapper::create(state, *this), true);
  }
  return 1;
}