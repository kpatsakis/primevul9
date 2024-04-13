  size_t removeInline(Handle handle) override {
    const size_t rc = header_map_->removeInline(handle);
    header_map_->verifyByteSizeInternalForTest();
    return rc;
  }