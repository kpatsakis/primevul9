    template <class Key> bool isPseudoHeader(const Key& key) {
      return !key.getStringView().empty() && key.getStringView()[0] == ':';
    }