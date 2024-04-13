  void appendInline(Handle handle, absl::string_view data, absl::string_view delimiter) override {
    ASSERT(handle.it_->second < inlineHeadersSize());
    HeaderEntry& entry = maybeCreateInline(&inlineHeaders()[handle.it_->second], handle.it_->first);
    addSize(HeaderMapImpl::appendToHeader(entry.value(), data, delimiter));
  }