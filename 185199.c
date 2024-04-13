  TestHeaderMapImplBase& operator=(const TestHeaderMapImplBase& rhs) {
    if (this == &rhs) {
      return *this;
    }
    clear();
    HeaderMapImpl::copyFrom(*header_map_, rhs);
    header_map_->verifyByteSizeInternalForTest();
    return *this;
  }