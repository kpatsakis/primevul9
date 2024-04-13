int DynamicMetadataMapWrapper::luaGet(lua_State* state) {
  const char* filter_name = luaL_checkstring(state, 2);
  const auto& metadata = streamInfo().dynamicMetadata().filter_metadata();
  const auto filter_it = metadata.find(filter_name);
  if (filter_it == metadata.end()) {
    return 0;
  }

  Filters::Common::Lua::MetadataMapHelper::createTable(state, filter_it->second.fields());
  return 1;
}