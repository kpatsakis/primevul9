  void appendInline(Handle handle, absl::string_view data, absl::string_view delimiter) override {
    header_map_->appendInline(handle, data, delimiter);
    header_map_->verifyByteSizeInternalForTest();
  }