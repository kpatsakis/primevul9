    template <class UnaryPredicate> void remove_if(UnaryPredicate p) {
      headers_.remove_if([&](const HeaderEntryImpl& entry) {
        const bool to_remove = p(entry);
        if (to_remove) {
          if (pseudo_headers_end_ == entry.entry_) {
            pseudo_headers_end_++;
          }
        }
        return to_remove;
      });
    }