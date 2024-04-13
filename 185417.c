    void finalizeTable() {
      CustomInlineHeaderRegistry::finalize<Interface::header_map_type>();
      auto& headers = CustomInlineHeaderRegistry::headers<Interface::header_map_type>();
      size_ = headers.size();
      for (const auto& header : headers) {
        this->add(header.first.get().c_str(), [&header](HeaderMapImpl& h) -> StaticLookupResponse {
          return {&h.inlineHeaders()[header.second], &header.first};
        });
      }
    }