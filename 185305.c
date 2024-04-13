  void setInline(Handle handle, uint64_t value) override {
    ASSERT(handle.it_->second < inlineHeadersSize());
    HeaderEntry& entry = maybeCreateInline(&inlineHeaders()[handle.it_->second], handle.it_->first);
    subtractSize(entry.value().size());
    entry.value().setInteger(value);
    addSize(entry.value().size());
  }