  void clear() override {
    header_map_->clear();
    header_map_->verifyByteSizeInternalForTest();
  }