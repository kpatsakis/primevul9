  void setReferenceInline(Handle handle, absl::string_view value) override {
    ASSERT(handle.it_->second < inlineHeadersSize());
    HeaderEntry& entry = maybeCreateInline(&inlineHeaders()[handle.it_->second], handle.it_->first);
    updateSize(entry.value().size(), value.size());
    entry.value().setReference(value);
  }