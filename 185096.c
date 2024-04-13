  void setInline(Handle handle, uint64_t value) override {
    header_map_->setInline(handle, value);
    header_map_->verifyByteSizeInternalForTest();
  }