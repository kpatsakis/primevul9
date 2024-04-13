    bool matchesHeaders(const HeaderMap& headers) const override {
      return HeaderUtility::matchHeaders(headers, *this);
    };