  TestHeaderMapImplBase(const std::initializer_list<std::pair<std::string, std::string>>& values) {
    for (auto& value : values) {
      header_map_->addCopy(LowerCaseString(value.first), value.second);
    }
    header_map_->verifyByteSizeInternalForTest();
  }