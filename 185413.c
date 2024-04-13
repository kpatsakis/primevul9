  void setReferenceInline(Handle handle, absl::string_view value) override {
    header_map_->setReferenceInline(handle, value);
    header_map_->verifyByteSizeInternalForTest();
  }