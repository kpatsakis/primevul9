  const HeaderEntry* getInline(Handle handle) const override {
    ASSERT(handle.it_->second < inlineHeadersSize());
    return constInlineHeaders()[handle.it_->second];
  }