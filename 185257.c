  void setInline(Handle handle, absl::string_view value) override {
    header_map_->setInline(handle, value);
    header_map_->verifyByteSizeInternalForTest();
  }