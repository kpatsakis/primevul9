  TestHeaderMapImplBase(const HeaderMap& rhs) {
    HeaderMapImpl::copyFrom(*header_map_, rhs);
    header_map_->verifyByteSizeInternalForTest();
  }