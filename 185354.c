  void removeJwt(Http::HeaderMap& headers) const override { headers.remove(header_); }