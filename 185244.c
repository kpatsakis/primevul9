  size_t removeInline(Handle handle) override {
    ASSERT(handle.it_->second < inlineHeadersSize());
    return HeaderMapImpl::removeInline(&inlineHeaders()[handle.it_->second]);
  }